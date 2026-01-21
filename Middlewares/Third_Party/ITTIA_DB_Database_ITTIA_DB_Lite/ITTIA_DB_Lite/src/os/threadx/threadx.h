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

#ifndef THREADX_H
#define THREADX_H

#include "ittia/os/os_config.h"

#if defined(OS_THREADX)

#include "ittia/os/threadx/threadx_thread.h"

C_HEADER_BEGIN

int os_threadx_init(void);
int os_threadx_done(void);

int os_threadx_error(UINT status);

#ifdef HAVE_FILEX_FILE
void * os_filex_get_media(void);
#endif

C_HEADER_END

#endif /* OS_THREADX */

#endif /* THREADX_H */
