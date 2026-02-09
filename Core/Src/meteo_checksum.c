/**
  ******************************************************************************
  * @file    meteo_checksum.c
  * @brief   METEO frame checksum validation (16-bit sum algorithm)
  * Adapted from meteo.c / meteo.h in PWRC2-CL2-AVR 1284P
  *   Checksum = sum of parsed integer values (temp + baro + wdir + wspeed + volt)  
  **  VERSION HISTORY:
  **  ----------------
  **  initial Version:    1.2b 
  **  Date:               11.4.22
  **  Current version:    08.02.26 
  **  Revised by: R.Oliva
  **  Description:
  **      - Hi-level meteo unit routines & data
  **      - Newer versions see top. 
  ******************************************************************************
  */

#include "meteo_checksum.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

/**
  * @brief  Calculate METEO checksum
  * @param  frame: Complete METEO frame string
  * @retval 16-bit checksum value (sum of parsed values)
  * 
  * Algorithm from original CL2 ATMega1284P code (2014):
  * - Parse 5 integer values: temp, baro, wdir, wspeed, voltage
  * - Checksum = temp + baro + wdir + wspeed + voltage
  * - Frames from METEO v20+ (>2014)
  * - Frame format: UUU$ttttt.bbbbb.dddd.sssss.vvv.CRCC*QQQ
  */
uint16_t meteo_calculate_checksum(const char *frame)
{
    int32_t temp = 0, baro = 0, wdir = 0, wspeed = 0, volt = 0;
    uint16_t chksum = 0;
    
    // Parse the 5 data values (skip the received checksum for now)
    // Format: UUU$ttttt.bbbbb.dddd.sssss.vvv.CRCC*QQQ
    int parsed = sscanf(frame, "UUU$%5d.%5d.%4d.%5d.%3d",
                        &temp, &baro, &wdir, &wspeed, &volt);
    
    if (parsed < 5)
    {
        return 0xFFFF;  // Parse failed
    }
    
    // Calculate checksum: sum of all parsed values
    chksum = (uint16_t)(temp + baro + wdir + wspeed + volt);
    
    return chksum;
}

/**
  * @brief  Validate METEO frame checksum
  * @param  frame: Complete METEO frame string
  * @retval 1 if valid, 0 if invalid
  */
int meteo_validate_checksum(const char *frame)
{
    int32_t temp = 0, baro = 0, wdir = 0, wspeed = 0, volt = 0;
    uint16_t received_chksum = 0;
    uint16_t calculated_chksum = 0;
    
    // Parse all 6 values: 5 data values + checksum
    // Format: UUU$ttttt.bbbbb.dddd.sssss.vvv.CRCC*QQQ
    int parsed = sscanf(frame, "UUU$%5d.%5d.%4d.%5d.%3d.%4hx",
                        &temp, &baro, &wdir, &wspeed, &volt, &received_chksum);
    
    if (parsed != 6)
    {
        return 0;  // Parse failed - need all 6 fields
    }
    
    // Calculate expected checksum: sum of 5 data values
    calculated_chksum = (uint16_t)(temp + baro + wdir + wspeed + volt);
    
    // Compare
    return (calculated_chksum == received_chksum);
}