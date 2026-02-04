/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_ittia.c
  * @author  ITTIA L.L.C.
  * @brief   ITTIA DB application implementation file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2005-2025 by ITTIA L.L.C. All rights reserved.
  *
  * This software is copyrighted by and is the sole property of ITTIA
  * L.L.C.  All rights, title, ownership, or other interests in the
  * software remain the property of ITTIA L.L.C.  This software may only
  * be used in accordance with the corresponding license agreement.  Any
  * unauthorized use, duplication, transmission, distribution, or
  * disclosure of this software is expressly forbidden.
  *
  * This Copyright notice may not be removed or modified without prior
  * written consent of ITTIA L.L.C.
  *
  * ITTIA L.L.C. reserves the right to modify this software without
  * notice.
  *
  * info@ittia.com
  * http://www.ittia.com
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include "app_ittia.h"
#include "stm32h5xx.h"

/* Private includes ----------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN TX_Pool_Buffer */
/* USER CODE END TX_Pool_Buffer */
#if defined ( __ICCARM__ )
#pragma data_alignment=4
#endif
// Static array in .bss (original)
__ALIGN_BEGIN static unsigned char db_mem_seg_buffer[DB_APP_MEM_SEG_BUFFER_SIZE] __ALIGN_END;

// To test sections:
// __ALIGN_BEGIN static unsigned char db_mem_seg_buffer[DB_APP_MEM_SEG_BUFFER_SIZE] __attribute__((section(".bss.ittia_db"))) __ALIGN_END;
// Or DTCM RAM:
// __ALIGN_BEGIN static unsigned char db_mem_seg_buffer[DB_APP_MEM_SEG_BUFFER_SIZE] __attribute__((section(".dtcm_ram"))) __ALIGN_END;
// NEW 2-2-26 11:19:
// static unsigned char *db_mem_seg_buffer = NULL;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

  /**
  * @brief  MX_ITTIA_Init - Replace 2-2-26 Instrumented
  * @param  None
  * @retval None
  */
void MX_ITTIA_Init(void)
{

  /* USER CODE BEGIN  MX_ITTIA_Init_1 */
  printf("\n=== MX_ITTIA_Init START ===\n");

  /* USER CODE END  MX_ITTIA_Init_1 */

  db_memory_config_t memory_config;
  db_init_t init_config;

  printf("Step 1: Clearing init_config...\n");
  memset(&init_config, 0, sizeof init_config);
  init_config.mem_config = &memory_config;
  printf("  init_config cleared OK\n");

  printf("Step 2: Setting up memory_config...\n");
  memory_config.num_segs = 0;
  memory_config.block_size = DB_APP_MEM_BLOCK_SIZE;
  memory_config.mem_flags = 0;
  printf("  Block size: %d bytes\n", DB_APP_MEM_BLOCK_SIZE);
  printf("  Buffer size: %lu bytes (%lu KB)\n",
         (unsigned long)DB_APP_MEM_SEG_BUFFER_SIZE,
         (unsigned long)(DB_APP_MEM_SEG_BUFFER_SIZE/1024));
  printf("  Buffer address: %p\n", (void*)db_mem_seg_buffer);

  /* Provide one of up to eight possible memory segments. */
  printf("Step 3: Configuring memory segment...\n");
  memory_config.mem_seg[memory_config.num_segs].ptr = db_mem_seg_buffer;
  memory_config.mem_seg[memory_config.num_segs].size = DB_APP_MEM_SEG_BUFFER_SIZE;
  ++memory_config.num_segs;
  printf("  Memory segment configured: num_segs=%d\n", memory_config.num_segs);

  /* Initialize ITTIA DB SQL library. */
  printf("Step 4: About to call db_init_ex()...\n");
  printf("  DB_API_VER: %d\n", DB_API_VER);
  printf("  Calling db_init_ex NOW...\n");

  dbstatus_t status_db;
  status_db = db_init_ex(DB_API_VER, &init_config);  // ← HANGS HERE

  printf("  db_init_ex RETURNED!\n");  // ← Will this print?
  printf("  Status: %d\n", status_db);

  if (DB_FAILED(status_db)) {
    /* USER CODE BEGIN  db_init_ex_Error */
    printf("ERROR: db_init_ex failed with status %d\n", status_db);
    while(1)
    {
    }
    /* USER CODE END  db_init_ex_Error */
  }

  /* USER CODE BEGIN  MX_ITTIA_Init_2 */
  printf("=== ITTIA DB initialized successfully! ===\n\n");
  /* USER CODE END  MX_ITTIA_Init_2 */
}


/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
