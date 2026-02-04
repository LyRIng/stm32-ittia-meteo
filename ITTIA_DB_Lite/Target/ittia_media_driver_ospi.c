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
/* USER CODE BEGIN Header */

/* USER CODE END Header */

#include "ittia_media_driver_ospi.h"

#include <stdint.h>

#include "tx_api.h"
#include "lx_stm32_ospi_driver.h" //  1.2.26 Added LevelX

static dbstatus_t check_ospi_status(uint64_t timeout);

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

static dbstatus_t ittia_media_ospi_init(void * driver_info, const char * storage_name, uint32_t open_create_flags, uint32_t * block_size, uint32_t * total_blocks)
{
    INT ret;
	ULONG ospi_block_size;
	ULONG ospi_total_blocks;
	lx_stm32_ospi_lowlevel_init(LX_STM32_OSPI_INSTANCE);
	if (check_ospi_status(TX_TIMER_TICKS_PER_SECOND) != DB_NOERROR)
	{
		return DB_EIO;
	}
	ret = lx_stm32_ospi_get_info(LX_STM32_OSPI_INSTANCE, &ospi_block_size, &ospi_total_blocks);
	if (ret != 0)
	{
		return DB_EIO;
	}
	LX_STM32_OSPI_POST_INIT();

	*block_size = ospi_block_size;
	*total_blocks = ospi_total_blocks;

	return DB_NOERROR;
}

static dbstatus_t ittia_media_ospi_shutdown(void * driver_info)
{
	if (0 != lx_stm32_ospi_lowlevel_deinit(LX_STM32_OSPI_INSTANCE)) {
		return DB_EIO;
	}

	return DB_NOERROR;
}

static dbstatus_t ittia_media_ospi_read_bytes(void * driver_info, void * region_info, uint64_t offset, void * data, uint32_t byte_count)
{
    dbstatus_t status = DB_NOERROR;

	if (check_ospi_status(LX_STM32_OSPI_DEFAULT_TIMEOUT) != DB_NOERROR)
	{
		return DB_EIO;
	}

	LX_STM32_OSPI_PRE_READ_TRANSFER(status);
	if (status != DB_NOERROR)
	{
		return status;
	}

	if (0 != lx_stm32_ospi_read(LX_STM32_OSPI_INSTANCE, (ULONG *)(uintptr_t)offset, (ULONG *)(uintptr_t)data, ((byte_count)/sizeof(ULONG))))
	{
		status = DB_EIO;
		LX_STM32_OSPI_READ_TRANSFER_ERROR(status);
	}
	else
	{
		LX_STM32_OSPI_READ_CPLT_NOTIFY(status);
	}

	LX_STM32_OSPI_POST_READ_TRANSFER(status);

	return status;
}

static dbstatus_t ittia_media_ospi_append_bytes(void * driver_info, void * region_info, uint64_t offset, const void * data, uint32_t byte_count)
{
    dbstatus_t status = DB_NOERROR;
    if ((NULL != data) && (check_ospi_status(((TX_TIMER_TICKS_PER_SECOND/100) * (byte_count * byte_count)) + (TX_TIMER_TICKS_PER_SECOND/100)) != DB_NOERROR))
    {
        return DB_EIO;
    }

    LX_STM32_OSPI_PRE_WRITE_TRANSFER(status);

    if (status != DB_NOERROR)
    {
        return status;
    }

    if (0 != lx_stm32_ospi_write(LX_STM32_OSPI_INSTANCE, (ULONG *)(uintptr_t)offset, (ULONG *)(uintptr_t)data, (byte_count/sizeof(ULONG))))
    {
        status = DB_EIO;
        LX_STM32_OSPI_WRITE_TRANSFER_ERROR(status);
    }
    else
    {
        LX_STM32_OSPI_WRITE_CPLT_NOTIFY(status);
    }

    LX_STM32_OSPI_POST_WRITE_TRANSFER(status);

	return status;
}

static dbstatus_t ittia_media_ospi_erase_block(void * driver_info, uint64_t block_number)
{
	if (0 != lx_stm32_ospi_erase(LX_STM32_OSPI_INSTANCE, block_number, 0, 0))
	{
		return DB_EIO;
	}

	return DB_NOERROR;
}

static dbstatus_t ittia_media_ospi_sync_writes(void * driver_info)
{
	/* The OSPI driver does not have any buffers to flush. */
	return DB_NOERROR;
}

static dbstatus_t check_ospi_status(uint64_t timeout)
{
    ULONG start = tx_time_get();
    while (tx_time_get() - start < timeout)
    {
        if (lx_stm32_ospi_get_status(LX_STM32_OSPI_INSTANCE) == 0)
        {
            return DB_NOERROR;
        }
    }

    return DB_ELOCKED;
}

const struct db_media_driver_s ittia_media_ospi = {
	.init         = &ittia_media_ospi_init,
	.shutdown     = &ittia_media_ospi_shutdown,
	.read_bytes   = &ittia_media_ospi_read_bytes,
	.append_bytes = &ittia_media_ospi_append_bytes,
	.erase_block  = &ittia_media_ospi_erase_block,
	.sync_writes  = &ittia_media_ospi_sync_writes,
};

/* USER CODE BEGIN Footer */

/* USER CODE END Footer */
