/* USER CODE BEGIN HeaderSim */
/**
  ******************************************************************************
  * @file           : meteo_simulator.h
  * @brief          : Header for meteo_simulator.c file.
  *                   Console-controlled METEO data simulator
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  ******************************************************************************
  */
/* USER CODE END HeaderSim */

#ifndef METEO_SIMULATOR_H
#define METEO_SIMULATOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"
#include <stddef.h>
#include <stdint.h>

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Initialize METEO simulator thread
 */
void meteo_simulator_init(void);

/**
 * @brief Toggle simulator on/off
 */
void meteo_simulator_toggle(void);

/**
 * @brief Check if simulator is currently enabled
 * @return 1 if enabled, 0 if disabled
 */
UINT meteo_simulator_is_enabled(void);

/**
 * @brief Generate simulated METEO frame with checksum
 * @param buffer Output buffer (minimum 64 bytes)
 * @param buf_size Size of output buffer
 */
void meteo_simulator_generate_frame(char *buffer, size_t buf_size);

/**
 * @brief Check console for simulator commands (non-blocking)
 */
void meteo_simulator_check_console(void);

#ifdef __cplusplus
}
#endif

#endif /* METEO_SIMULATOR_H */