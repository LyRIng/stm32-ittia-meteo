/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team / R.Oliva
  * @brief   ThreadX applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  * Version updated for IDC_ENABLED 11.2.26
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

// Added 27.1.26
#include "meteo_thread.h"

// *** NEW: ITTIA DB Integration ***
#include "app_ittia.h"
#include "meteo_example.h"
#include "ittia_media_driver_ospi.h"

// *** NEW: Network for Analitica sync ***
#include "db_netxduo_tcp.h"
#include "app_netxduo.h"  // Your NetX Duo configuration

//10.2.26 Simulator
#include "meteo_simulator.h"

// 13.2.26 Include Buffer Sizes in main.h for queues
// --> for METEO_QUEUE_STORAGE_SIZE
#include "main.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE BEGIN Before_Kernel_Start */
#undef TX_THREAD_IDLE_STACK_SIZE
#define TX_THREAD_IDLE_STACK_SIZE 2048  // or 4096
/* USER CODE END Before_Kernel_Start */

// *** NEW: Configuration ***
#define METEO_IDC_ENABLED  0  // Set to 1 to enable Analitica sync - testing 11/2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD tx_app_thread;
/* USER CODE BEGIN PV */

// *** NEW: ITTIA DB driver info ***
static ittia_media_ospi_info_t driver_info;
extern XSPI_HandleTypeDef hospi1;  // From CubeMX

/* *** 12.2.26 added METEO database processing thread *** */
TX_THREAD meteo_db_thread;
UCHAR meteo_db_thread_stack[2048];

// *** NEW: IDC agent thread (if enabled) ***
#if METEO_IDC_ENABLED
TX_THREAD idc_agent_thread;
UCHAR idc_agent_thread_stack[4096];
#endif

// *** NEW: Use YOUR actual network instances from app_netxduo.c ***
extern NX_IP NetXDuoEthIpInstance;      // Your IP instance name
extern NX_PACKET_POOL NxAppPool;         // Your packet pool name
extern TX_SEMAPHORE DHCPSemaphore;       // Your DHCP semaphore

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* *** 12.2.26: Database processing thread (See end of file) *** */
static void meteo_db_thread_entry(ULONG thread_input);

#if METEO_IDC_ENABLED
static void idc_agent_thread_entry(ULONG thread_input);
#endif

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  /* USER CODE END App_ThreadX_MEM_POOL */
  
  
  /* USER CODE BEGIN App_ThreadX_Init */

  /* *** 12-02-26 Create METEO frame queue (before threads) *** */
  /* Queue and storage global in main.c                         */
  extern TX_QUEUE meteo_frame_queue;
  extern UCHAR meteo_queue_storage[];
  
  // Diagnostic - 13.2.26 before queue creation
  printf("=== Queue Debug Info ===\n");
  printf("  sizeof(ULONG): %d\n", sizeof(ULONG));
  printf("  RX_BUFFER_SIZE: %d\n", RX_BUFFER_SIZE);
  printf("  Message size calculation: %d / %d = %d\n",
         RX_BUFFER_SIZE, sizeof(ULONG), RX_BUFFER_SIZE / sizeof(ULONG));

  // Check if message size is valid
  if ((RX_BUFFER_SIZE % sizeof(ULONG)) != 0)
  {
    printf("WARNING: RX_BUFFER_SIZE not aligned to ULONG!\n");
  }

  // Verify storage
  printf("  Required storage: %d messages * %d bytes = %d\n",
         METEO_QUEUE_SIZE, RX_BUFFER_SIZE, METEO_QUEUE_SIZE * RX_BUFFER_SIZE);
  printf("  Allocated storage: %d\n", METEO_QUEUE_STORAGE_SIZE);

  if (METEO_QUEUE_STORAGE_SIZE < (METEO_QUEUE_SIZE * RX_BUFFER_SIZE))
  {
    printf("ERROR: Storage too small!\n");
  }

  // 13.2.26 - use QUEUE size defined in main.h
  if (tx_queue_create(&meteo_frame_queue,
                      "METEO Frame Queue",
					  RX_BUFFER_SIZE / sizeof(ULONG),  // Message size in ULONGs 13.2.26
                      meteo_queue_storage,
					  METEO_QUEUE_STORAGE_SIZE) != TX_SUCCESS)
  {
    printf("ERROR: Failed to create METEO frame queue\n");
    return TX_QUEUE_ERROR;
  }
  
  
  /* Declare thread and stack as static or extern */
  extern TX_THREAD meteo_thread;
  // recommended 27.1. use UCHAR instead of uint8_t

  // See how sizes should be defined
  extern UCHAR meteo_thread_stack[];
  //void Meteo_Thread_Entry(ULONG thread_input);

  /* Create meteo thread INSIDE ThreadX */
  if (tx_thread_create(&meteo_thread, "Meteo Thread",
                       Meteo_Thread_Entry, 0,
                       meteo_thread_stack, sizeof(meteo_thread_stack),
                       10, 10, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
  
  /* *** 12.02.26 NEW: Create METEO database processing thread *** */
  extern TX_THREAD meteo_db_thread;
  extern UCHAR meteo_db_thread_stack[];
  
  if (tx_thread_create(&meteo_db_thread,
                       "METEO DB Thread",
                       meteo_db_thread_entry,
                       0,
                       meteo_db_thread_stack,
                       sizeof(meteo_db_thread_stack),
                       15,  // Priority
                       15,
                       TX_NO_TIME_SLICE,
                       TX_AUTO_START) != TX_SUCCESS)
  {
    printf("[WARNING] METEO DB thread creation failed\n");
    // Continue anyway
  }
  

#if METEO_IDC_ENABLED
  // *** Create IDC agent thread for Analitica synchronization ***
  if (tx_thread_create(&idc_agent_thread, "IDC Agent", idc_agent_thread_entry, 0,
                       idc_agent_thread_stack, sizeof(idc_agent_thread_stack),
                       16, 16, TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    printf("[WARNING] IDC agent thread creation failed - Analitica sync disabled\n");
    // Continue anyway - local storage will still work
  }
#endif

  /* USER CODE END App_ThreadX_Init */

  CHAR *pointer;

  /* Allocate the stack for tx app thread  */
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
                       TX_APP_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  /* Create tx app thread.  */
  if (tx_thread_create(&tx_app_thread, "tx app thread", tx_app_thread_entry, 0, pointer,
                       TX_APP_STACK_SIZE, TX_APP_THREAD_PRIO, TX_APP_THREAD_PREEMPTION_THRESHOLD,
                       TX_APP_THREAD_TIME_SLICE, TX_APP_THREAD_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN App_ThreadX_Init */
  /* USER CODE END App_ThreadX_Init */

  return ret;
}
/**
  * @brief  Function implementing the tx_app_thread_entry thread.
  * @param  thread_input: Hardcoded to 0.
  * @retval None
  */
void tx_app_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN tx_app_thread_entry */
  // 2.2.26 MX_ITTIA_Init() should go in main, before ThreadX
  // printf("\n=== METEO Weather Station with ITTIA DB ===\n");
  printf("\n=== METEO Weather Station - Starting Services ===\n");
  
  printf("Waiting for network initialization...\n");
  tx_thread_sleep(200);


  // 9.2.26 NEW: Initialize MeteoSimulator thread (internal message if fails)
  meteo_simulator_init();

  printf("Services started - press 'S' to toggle simulator\n");

  // *** NEW: Initialize OSPI driver for database storage ***
  driver_info.hspi = hospi1;

  // *** NEW: Initialize METEO example (creates stream environment) ***
  printf("Initializing METEO streams...\n");
  if (meteo_example_init(&ittia_media_ospi, &driver_info) != EXIT_SUCCESS)
  {
    printf("[ERROR] METEO example initialization failed!\n");
    Error_Handler();
  }

  // *** NEW: Run METEO example (sets up streams) ***
  printf("Setting up METEO database streams...\n");
  if (run_meteo_example(&ittia_media_ospi, &driver_info) != EXIT_SUCCESS)
  {
    printf("[ERROR] METEO example setup failed!\n");
    Error_Handler();
  }

  printf("=== METEO system ready ===\n");  // update menu 11.2.26
  printf("  - Real sensor: Connect METEO to UART3\n");
  printf("  - Simulator: Press 'S' to toggle\n");
  
  #if METEO_IDC_ENABLED
    printf("  - Analitica: IDC agent running in background\n");
  #else
    printf("  - Analitica: Disabled (set METEO_IDC_ENABLED=1)\n");
  #endif

  // *** Main thread entering - UART ISR handles data ***
  // No while loop here.. 10/2/26
  //while (1)
  //{
  //tx_thread_sleep(1000);  // Sleep 1 second
  //}

  /* USER CODE END tx_app_thread_entry */
}

#if METEO_IDC_ENABLED
/**
  * @brief  IDC Agent thread entry - syncs data to Analitica
  * @param  thread_input: Not used
  * @retval None
  */
static void idc_agent_thread_entry(ULONG thread_input)
{
  ULONG actual_status;
  ULONG ip_address;
  ULONG network_mask;

  /* Wait for DHCP to assign IP address */
  printf("IDC Agent: Waiting for network initialization (DHCP)...\n");

  /* Wait on DHCP semaphore - this is set when IP is assigned */
  if (tx_semaphore_get(&DHCPSemaphore, TX_TIMER_TICKS_PER_SECOND * 30) != TX_SUCCESS)
  {
    printf("[ERROR] IDC Agent: DHCP timeout - no IP address assigned\n");
    printf("[INFO] Check Ethernet cable and DHCP server\n");
    /* Continue anyway - might work with static IP */
  }

  /* Get the IP address */
  if (nx_ip_address_get(&NetXDuoEthIpInstance, &ip_address, &network_mask) == NX_SUCCESS)
  {
    printf("IDC Agent: IP address assigned: %lu.%lu.%lu.%lu\n",
           (ip_address >> 24) & 0xFF,
           (ip_address >> 16) & 0xFF,
           (ip_address >> 8) & 0xFF,
           ip_address & 0xFF);
  }
  else
  {
    printf("[WARNING] IDC Agent: Could not get IP address\n");
  }

  /* Wait a bit more for link to stabilize */
  tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 2);

  /* Register NetX Duo TCP protocol */
  DB_NETXDUO_TCP_REGISTER();
  printf("IDC Agent: TCP protocol registered\n");

  /* Configure TCP connection to Analitica using YOUR network instances */
  db_netxduo_tcp_options_t tcp_options = {
    .size = sizeof(db_netxduo_tcp_options_t),
    .netxduo_packet_pool = &NxAppPool,              // YOUR packet pool
    .netxduo_tcp_interface = &NetXDuoEthIpInstance, // YOUR IP instance
    .netxduo_tcp_port = 16534  // Default ITTIA DB port
  };

  printf("IDC Agent: Starting - connecting to Analitica on port 16534...\n");
  printf("IDC Agent: Waiting for Analitica to connect...\n");

  /* Run IDC agent (blocks, retries on error) */
  run_meteo_idc_agent("idb+tcp", &tcp_options);

  /* Should never reach here */
  printf("[ERROR] IDC Agent unexpectedly terminated\n");
}
#endif

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN Before_Kernel_Start */
#undef TX_THREAD_IDLE_STACK_SIZE
#define TX_THREAD_IDLE_STACK_SIZE 2048  // or 4096
  /* USER CODE END Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN Kernel_Start_Error */

  /* USER CODE END Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
/**
 * @brief METEO database processing thread 13.2.26
 * Receives frames from UART ISR queue and stores in database
 */
static void meteo_db_thread_entry(ULONG thread_input)
{
    (void)thread_input;
    char frame_buffer[RX_BUFFER_SIZE];  // 13.2.26 RX_BUFFER_SIZE from main.H
    extern TX_QUEUE meteo_frame_queue;
    
    printf("[DB Thread] Started - waiting for METEO frames\n");
    
    while(1)
    {
        // Wait for frame from queue (blocks until available)
        if (tx_queue_receive(&meteo_frame_queue, frame_buffer, TX_WAIT_FOREVER) == TX_SUCCESS)
        {
            // Now safe to call ITTIA DB functions (thread context!)
            ProcessMeteoFrameToStream(frame_buffer);
        }
    }
}

/* USER CODE END 1 */
