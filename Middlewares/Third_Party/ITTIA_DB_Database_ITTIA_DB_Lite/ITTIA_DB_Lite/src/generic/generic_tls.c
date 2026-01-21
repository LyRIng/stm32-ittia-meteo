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

#if defined(HAVE_THREADS) && defined(HAVE_GENERIC_TLS)

#include "ittia/os/os_tls.h"
#include "ittia/os/os_thread.h"
#include "ittia/os/os_error.h"
#include "ittia/os/os_malloc.h"

#include "os/os_mockup.h"


typedef struct os_tls_generic_node_t {
	uintptr_t thread_id;
	os_tls_value_t value;
	struct os_tls_generic_node_t* next;
} os_tls_generic_node_t;


DBDLL_API int os_tls_init(os_tls_key_t * key)
{
    // TODO: a recursive call is possible if a DB_MALLOC failure is responsible
    // for the first error on the current thread.
	os_tls_generic_node_t* node = (os_tls_generic_node_t*) DB_MALLOC(sizeof(os_tls_generic_node_t));
	node->thread_id = os_thread_id();
	node->value = NULL;
	node->next = NULL;
	*key = node;
	return DB_NOERROR;
}

DBDLL_API int os_tls_destroy(os_tls_key_t * key)
{
	if (*key == NULL)
		return DB_EINVAL;

	/* Free all nodes in the list. */
	while (*key != NULL) {
		os_tls_generic_node_t* node = *key;
		*key = (*key)->next;
		DB_FREE(node);
	}
	return DB_NOERROR;
}

DBDLL_API int os_tls_set(os_tls_key_t key, os_tls_value_t value)
{
	os_tls_generic_node_t* node = key;
	uintptr_t thread_id = os_thread_id();

	if (node == NULL)
		return DB_EINVAL;

	/* Search for thread in the list. */
	while (1) {
		if (node->thread_id == thread_id) {
			node->value = value;
			return DB_NOERROR;
		}
		if (node->next == NULL)
			break;
		node = node->next;
	}

	/* Thread was not found in list, so add it to the end. */
	os_tls_init(&(node->next));
	node->next->value = value;
	return DB_NOERROR;
}

DBDLL_API int os_tls_get(os_tls_key_t key, os_tls_value_t * value)
{
	os_tls_generic_node_t* node = key;
	uintptr_t thread_id = os_thread_id();

	if (node == NULL)
		return DB_EINVAL;

	/* Search for thread in the list. */
	while (node != NULL) {
		if (node->thread_id == thread_id) {
			*value = node->value;
			return DB_NOERROR;
		}
		node = node->next;
	}
	/* If thread is not found, value is NULL. */
	*value = NULL;
	return DB_NOERROR;
}

#endif /* HAVE_POSIX_TLS */
