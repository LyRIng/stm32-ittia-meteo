/**
  ******************************************************************************
  * @file    meteo_checksum.h
  * @brief   Header for METEO frame checksum validation (16-bit sum algorithm)
  * Adapted from meteo.c / meteo.h in PWRC2-CL2-AVR 1284P
  *   Checksum = sum of parsed integer values (temp + baro + wdir + wspeed + volt) 
  ******************************************************************************
  */
  
#ifndef METEO_CHECKSUM_H
#define METEO_CHECKSUM_H

#include <stdint.h>

/**
 * @brief Calculate METEO checksum (16-bit sum)
 * @param frame Complete METEO frame string
 * @return 16-bit checksum value of parsed values
 */
uint16_t meteo_calculate_checksum(const char *frame);

/**
 * @brief Validate METEO frame checksum
 * @param frame Complete METEO frame string
 * @return 1 if valid, 0 if invalid
 */
int meteo_validate_checksum(const char *frame);

#endif