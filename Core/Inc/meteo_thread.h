/*
 * meteo_thread.h
 *
 *  Created on: Jan 27, 2026
 *      Author: R:Oliva /Claude
 */

#ifndef INC_METEO_THREAD_H_
#define INC_METEO_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"

/* Exported variables --------------------------------------------------------*/
extern TX_THREAD meteo_thread;
extern UCHAR meteo_thread_stack[1024];

/* Exported functions --------------------------------------------------------*/
void Meteo_Thread_Entry(ULONG thread_input);

#ifdef __cplusplus
}
#endif

#endif /* INC_METEO_THREAD_H_ */
