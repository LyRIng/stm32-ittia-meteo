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

#ifndef ITTIA_MEDIA_DRIVER_OSPI_H
#define ITTIA_MEDIA_DRIVER_OSPI_H

#include <ittia/ittiadb_lite/ittia_media_driver.h>

#include "stm32h5xx_hal.h"
#include "stm32h5xx_ll_dlyb.h"       // ← ADD THIS LINE 1.2.26 afternoon
#include "stm32h5xx_hal_xspi.h"      // ← ADDED THIS LINE 1.2.26


typedef struct ittia_media_ospi_info_s {
	XSPI_HandleTypeDef hspi;
} ittia_media_ospi_info_t;

extern const struct db_media_driver_s ittia_media_ospi;

#endif // ITTIA_MEDIA_DRIVER_OSPI_H

