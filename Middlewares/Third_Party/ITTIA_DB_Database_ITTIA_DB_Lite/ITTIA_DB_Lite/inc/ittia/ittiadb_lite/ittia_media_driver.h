/**************************************************************************/
/*                                                                        */
/*      Copyright (c) 2005-2025 by ITTIA L.L.C. All rights reserved.      */
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

#ifndef ITTIA_MEDIA_DRIVER_H
#define ITTIA_MEDIA_DRIVER_H

#include <ittia/os/os_error.h>
#include <ittia/db/db_index_storage.h>

#ifdef __cplusplus
extern "C" {
#endif

struct db_media_driver_s
{
	dbstatus_t (*init)(void * driver_info, const char * storage_name, uint32_t open_create_flags, uint32_t * block_size, uint32_t * total_blocks);
	dbstatus_t (*shutdown)(void * driver_info);
	dbstatus_t (*read_bytes)(void * driver_info, void * region_info, uint64_t offset, void * data, uint32_t byte_count);
	dbstatus_t (*append_bytes)(void * driver_info, void * region_info, uint64_t offset, const void * data, uint32_t byte_count);
	dbstatus_t (*erase_block)(void * driver_info, uint64_t block_number);
	dbstatus_t (*sync_writes)(void * driver_info);
};


#ifdef __cplusplus
}
#endif

#endif // ITTIA_MEDIA_DRIVER_H
