/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2023 by ITTIA L.L.C. All rights reserved.      */
/*                                                                        */
/*  This software is copyrighted by and is the sole property of ITTIA     */
/*  L.L.C.  All rights, title, ownership, or other interests in the       */
/*  software remain the property of ITTIA L.L.C.  This software may only  */
/*  be used in accordance with the corresponding license agreement.  Any  */
/*  unauthorized use, duplication, transmission, distribution, or         */
/*  disclosure of this software is expressly forbidden.                   */
/*                                                                        */
/*  This Copyright notice may not be removed or modified without prior    */
/*  written consent of ITTIA L.L.C.                                       */
/*                                                                        */
/*  ITTIA L.L.C. reserves the right to modify this software without       */
/*  notice.                                                               */
/*                                                                        */
/*  info@ittia.com                                                        */
/*  https://www.ittia.com                                                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

#include "ittia/os/os_config.h"

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_CONDVAR)

#include "ittia/os/os_condvar.h"
#include "ittia/os/os_mutex.h"
#include "ittia/os/os_debug.h"
#include "ittia/os/std/limits.h"
#include "ittia/os/std/memory.h"

#ifdef DB_DEBUG
#include "ittia/os/os_atomic.h"
#endif

#include "os/os_mockup.h"

/*
 * CV taken from USENET posts by Alexander Terekhov <TEREKHOV@de.ibm.com>.
 *
 * Oleg Sesov wrote:
 * > May I use your algorithms in commercial application? I mean your posts
 * > in USENET about condition variables and rw locks, which have no
 * > disclaimers.
 *
 * Yes, AFAIK it is patent free.
 */

//#define NO_SPURIOUS_WAKEUPS

#ifdef DB_DEBUG

static os_atomic_t cur_cond_count = 0;

#define os_cond_use()       os_atomic_inc( &cur_cond_count )
#define os_cond_release()   os_atomic_dec( &cur_cond_count )

long os_cond_used(void)
{
    return os_atomic_fetch( &cur_cond_count );
}

#else
#define os_cond_use()
#define os_cond_release()

#endif

#define CV_INITIALIZED  50505

int os_cond_init(os_cond_t* cv)
{
    memset( cv, 0, sizeof(*cv) );
    DB_THROW_ERROR( os_mutex_init(&cv->leave_lock) );
    DB_THROW_ERROR( os_sem_init(&cv->entry_lock, 1) );
    DB_THROW_ERROR( os_sem_init(&cv->queue, 0) );

    cv->gone = 0;
    cv->sleeping = 0;
    cv->wakeup = 0;
    cv->self = cv;
    cv->state = CV_INITIALIZED;

    os_cond_use();

    return DB_NOERROR;
}

int os_cond_destroy(os_cond_t* cv)
{
    if (cv->state == CV_INITIALIZED) {
        DB_ASSERT( cv->self == cv );
        DB_REQUIRE( cv->sleeping == cv->gone, DB_ELOCKED );
        DB_REQUIRE( cv->wakeup == 0, DB_ELOCKED );

        os_mutex_destroy( &cv->leave_lock );
        os_sem_destroy( &cv->entry_lock );
        os_sem_destroy( &cv->queue );

        os_cond_release();

        cv->state = 0;
        cv->self = 0;
    }

    return DB_NOERROR;
}

static int os_cond_signal_core(os_cond_t * cv, int broadcast)
{
    int signals = 0;
    int rc;

    DB_ASSERT( cv->self == cv );
    DB_ASSERT( cv->state == CV_INITIALIZED );

    /* don't allow any waiter to leave signal section */
    THROW_ERROR( os_mutex_lock( &cv->leave_lock ) );

    /* check if the variable already processing some signals */
    if ( cv->wakeup != 0 ) { // the gate is closed!!!

        /* if there are no unconsidered waiters we post nothing */
        if ( cv->sleeping == 0 ) {
            /* all the waiting threads awaken */
            return os_mutex_unlock( &cv->leave_lock );
        }

        /* wake up appropriate number of waiters */
        if (broadcast)
            signals = cv->sleeping;
        else
            signals = 1;

        cv->wakeup += signals;
        cv->sleeping -= signals;

    } else if ( cv->sleeping > cv->gone ) { // HARMLESS RACE CONDITION!
        /* this means that int loading/storing operations are atomic.
         * Counterpart of the race condition is in sleeping
         * increment in wait().
         */

        /* since we initiate new signal/broadcast processing round
         * we prohibit any new waiters to come in while the processing
         * is not finished.
         */
        if (DB_FAILED( rc = os_sem_wait( &cv->entry_lock, OS_WAIT_FOREVER ))) // close the gate
        {
            os_mutex_unlock( &cv->leave_lock );
            return rc;
        }

        /* adjust biased number of waiters, because we reuse the variable for
         * spurious wakeups starting from now on.
         */
        if ( cv->gone != 0 ) {
            cv->sleeping -= cv->gone;
            cv->gone = 0;
        }

        if (broadcast)
            signals = cv->sleeping;
        else
            signals = 1;

        cv->wakeup = signals;
        cv->sleeping -= signals;

    } else {
        /* no waiters yet */
    }
    os_mutex_unlock( &cv->leave_lock );
    /* wakeup threads */
    if (signals)
        os_sem_post( &cv->queue, signals );

    return DB_NOERROR;
}

int os_cond_signal(os_cond_t* cv)
{
    return os_cond_signal_core(cv, 0);
}

int os_cond_broadcast(os_cond_t* cv)
{
    return os_cond_signal_core(cv, 1);
}

int os_cond_wait( os_cond_t * cv, os_mutex_t * mutex, os_wait_time_t timeout )
{
    int result;
    int waiters_left;
#ifdef NO_SPURIOUS_WAKEUPS
    int missed_wakeups;
#endif

    DB_ASSERT( cv->self == cv );
    DB_ASSERT( cv->state == CV_INITIALIZED );

    THROW_ERROR( os_sem_wait( &cv->entry_lock, OS_WAIT_FOREVER ) );
    cv->sleeping++;

    /* following sem_post() opens the signal section.
     * Note that sleeping - gone is always equal to
     * the number of threads in signal section, each of them is a
     * subject to recieve a signal or to timeout it's wait.
     */
    os_sem_post( &cv->entry_lock, 1 );
    os_mutex_unlock( mutex );

    result = os_sem_wait( &cv->queue, timeout );

  /*
   * Whether we got here as a result of signal/broadcast or because of
   * timeout on wait or thread cancellation we indicate that we are no
   * longer waiting. The waiter is responsible for adjusting waiters
   * (to)unblock(ed) counts (protected by unblock lock).
   */

    /* following lock ends signal section */
    os_mutex_lock( &cv->leave_lock );

    waiters_left = cv->wakeup;

    if ( cv->wakeup != 0 ) {
        /* signal/broadcast is being processed */
        if ( result == DB_ELOCKED ) {
            /* check if current thread is the latest one to leave the
             * signal section
             */
            if ( cv->sleeping != 0 )
                /* there are more threads in signal section waiting,
                 * that means we are processing signal, thus we timedout
                 * by all means.
                 * we can safely adjust unblock counter, because the last
                 * woken thread would count this as a bias to a number
                 * of waiters.
                 */
                cv->sleeping--; 
            else
                /* we are processing broadcast or current thread is the
                 * latest who leave signal section. Adjust the semaphore
                 * and let the latest thread to do it.
                 */
                cv->gone++;
        }

        --cv->wakeup;
        if ( cv->wakeup == 0 ) {

            /* signal processing has been completed, current thread is
             * the last one who has signal sent (or timed out)
             */
            if ( cv->sleeping != 0 ) {
#ifdef NO_SPURIOUS_WAKEUPS
                /* there are more threads in signal section, do
                 * not adjust the semaphore.
                 */
                missed_wakeups = 0;
                /* here we can open input gate, because there is no
                 * spurious wakeups
                 */
#endif
            }
#ifdef NO_SPURIOUS_WAKEUPS
            else if ( cv->gone != 0 ) {
                /* it is known there are no any thread in signal section
                 * thus we are adjust the queue semaphore
                 */
                missed_wakeups = cv->gone;
                /* here we can't open the hate, because there are spurious
                 * wakeups pending, so we throw them away first
                 */
            }
#endif
            cv->gone = 0;
        }
    }
    else {
        ++cv->gone;

        if ( cv->gone == INT_MAX/2 ) {
            /* In fact gone serves for two purposes.
             *
             * 1) While we are processing a broadcast it counts number of
             *    lost wakeups.
             *
             * 2) reduce number of locks on entry_lock while we don't
             *    handle a signal or broadcast. Thus we are biasing the number
             *    of waiters as follows:
             *
             *       actual number of waiters = sleeping - gone;
             *
             * This branch is actually the second purpose, so I am unsure
             * that the following original comment means actually.
             */

            // timeout/canceled or spurious semaphore :-)
            // something is going on here - test of timeouts? :-)

            os_sem_wait( &cv->entry_lock, OS_WAIT_FOREVER );
            cv->sleeping -= cv->gone;
            os_sem_post( &cv->entry_lock, 1 );
            cv->gone = 0;
        }
    }

    os_mutex_unlock( &cv->leave_lock );

    if ( waiters_left == 1 ) {

#ifdef NO_SPURIOUS_WAKEUPS
        if ( missed_wakeups != 0 ) {

            while ( missed_wakeups-- )
                os_sem_wait( &cv->queue );          // better now than spurious later

        }
#endif
        os_sem_post( &cv->entry_lock, 1 );              // open the gate

    }

    os_mutex_lock( mutex );

    return result;
}

#endif
