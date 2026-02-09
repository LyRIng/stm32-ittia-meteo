/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : R.Oliva Testing UART3 with ThreadX 01.26
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"
#include "main.h"
#include "dcache.h"
#include "eth.h"
#include "icache.h"
#include "memorymap.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* Includes (add these if not already present) ok 14-01-26 */
#include "stm32h5xx_hal.h"
// Added 3.3.26
#include "app_ittia.h"
// #include "stm32h573i_discovery_lcd.h"   // or your BSP_LCD header 14-1-26 off
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// To avoid linker errors
// using int _gettimeofday(struct timeval *tv, void *tzvp) in USER CODE 4
#include <sys/time.h>
#include <errno.h>

// Added 27.1.26
#include "meteo_thread.h"

// *** NEW: Added for ITTIA DB integration ***
#include "meteo_example.h"

// 8.2.26 New checksum validation - Based on old CL2 (2014) -8-bit meteo.c/.h
#include "meteo_checksum.h"  

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Private defines 13-01-26 */
#define RX_BUFFER_SIZE          128
#define FRAME_START             "UUU$"
#define FRAME_END               "*QQQ"
#define MAX_FRAME_LEN           80

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

COM_InitTypeDef BspCOMInit;

/* USER CODE BEGIN PV */

/* 1-2-26- OSPI */
XSPI_HandleTypeDef hospi1;              // ← ADD THIS LINE 2/2/26
// DMA 3/2/26 from workin main.c
DMA_HandleTypeDef handle_GPDMA1_Channel1;
DMA_HandleTypeDef handle_GPDMA1_Channel0;

/* Private variables 13-01-26 */
extern UART_HandleTypeDef huart3;       // From CubeMX
// extern UART_HandleTypeDef huart1;       // VCP

static uint8_t rxByte;
static char rxBuffer[RX_BUFFER_SIZE];
static uint8_t rxIndex = 0;
static uint8_t frameInProgress = 0;
static uint32_t lastReceptionTick = 0;

/* 17.1.26 ThreadX variables for meteo thread */
TX_THREAD meteo_thread;
// recommended 27.1. use UCHAR instead of uint8_t
//#define METEO_THREAD_STACK_SIZE   2048   // This is fine 2/2/26
UCHAR meteo_thread_stack[2048];  // Small stack for minimal thread
// uint8_t meteo_thread_stack[1024]; not ThreadX Native type

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Forward declarations 13.01.26 */
void ProcessMeteoFrame(const char* frame);
int ChecksumValidate(const char* frame);   // implement your CRCC logic

// 17.1.26 Thread Entry function
// 27.1 in meteo_thread.h
// static void Meteo_Thread_Entry(ULONG thread_input);  // Thread entry function
// Added 2/2/26 OCTO_SPI Init
static void MX_OCTOSPI1_Init(void);     // ← ADD THIS LINE 2/2
static void MX_GPDMA1_Init(void);       // ← Added 2/2/ 19:55
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init(); // Added 2.2.26 Last resource
  MX_ICACHE_Init();
  // 2.2.26 - OCTO_SPIInit
  MX_OCTOSPI1_Init();        // ← ADD THIS LINE -2.2-26
  MX_DCACHE1_Init();

  MX_ETH_Init();
  MX_USART3_UART_Init();



  /* USER CODE BEGIN 2 */

  //- 14-1-26 NO COM Init yet here
  // MX_ThreadX_Init();
  /* USER CODE END 2 */

  //MX_ThreadX_Init();

  /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
  BspCOMInit.BaudRate   = 115200;
  BspCOMInit.WordLength = COM_WORDLENGTH_8B;
  BspCOMInit.StopBits   = COM_STOPBITS_1;
  BspCOMInit.Parity     = COM_PARITY_NONE;
  BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
  if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("COM - Ok Connection STM32H573! \r\n");
  // eliminate Task creation here, move to app_threadx.c (Claude 27.1.26)

  printf("\n=== METEO Weather Station with ITTIA DB ===\n");

  // *** NEW: Initialize ITTIA DB ***
  printf("Initializing ITTIA DB...\n");
  MX_ITTIA_Init();

  printf("Starting ThreadX kernel\r\n");
  /* Initialize ThreadX 17.1.26 */
  MX_ThreadX_Init();

  // All rest eliminated 27.1.26 Claude

  /* Infinite loop */
  while (1)
  {
   //Scheduler on 17.1.26
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  {
	// No user code after While 13.01.26
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS_DIGITAL;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLL1_SOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 5;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1_VCIRANGE_2;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1_VCORANGE_WIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the programming delay
  */
  __HAL_FLASH_SET_PROGRAM_DELAY(FLASH_PROGRAMMING_DELAY_2);
}

// Added 2/2/26
/**
  * @brief OCTOSPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_OCTOSPI1_Init(void)
{
  /* USER CODE BEGIN OCTOSPI1_Init 0 */

  /* USER CODE END OCTOSPI1_Init 0 */

  HAL_XSPI_DLYB_CfgTypeDef HAL_OSPI_DLYB_Cfg_Struct = {0};

  /* USER CODE BEGIN OCTOSPI1_Init 1 */

  /* USER CODE END OCTOSPI1_Init 1 */

  /* OCTOSPI1 parameter configuration*/
  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThresholdByte = 1;
  hospi1.Init.MemoryMode = HAL_XSPI_SINGLE_MEM;
  hospi1.Init.MemoryType = HAL_XSPI_MEMTYPE_MACRONIX;
  hospi1.Init.MemorySize = HAL_XSPI_SIZE_512MB;
  hospi1.Init.ChipSelectHighTimeCycle = 2;
  hospi1.Init.FreeRunningClock = HAL_XSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_XSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_XSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 3;
  hospi1.Init.SampleShifting = HAL_XSPI_SAMPLE_SHIFT_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_XSPI_DHQC_ENABLE;
  hospi1.Init.ChipSelectBoundary = HAL_XSPI_BONDARYOF_NONE;
  hospi1.Init.DelayBlockBypass = HAL_XSPI_DELAY_BLOCK_ON;
  hospi1.Init.Refresh = 0;
  if (HAL_XSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_OSPI_DLYB_Cfg_Struct.Units = 56;
  HAL_OSPI_DLYB_Cfg_Struct.PhaseSel = 2;
  if (HAL_XSPI_DLYB_SetConfig(&hospi1, &HAL_OSPI_DLYB_Cfg_Struct) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN OCTOSPI1_Init 2 */

  /* USER CODE END OCTOSPI1_Init 2 */
}

// 2.2.26 dma1 ADDED From ITTIA_Nx_NoR_NanoEdgeAI
/**
  * @brief GPDMA1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPDMA1_Init(void)
{

  /* USER CODE BEGIN GPDMA1_Init 0 */

  /* USER CODE END GPDMA1_Init 0 */

  /* Peripheral clock enable */
  __HAL_RCC_GPDMA1_CLK_ENABLE();

  /* GPDMA1 interrupt Init */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
    HAL_NVIC_SetPriority(GPDMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel1_IRQn);

  /* USER CODE BEGIN GPDMA1_Init 1 */

  /* USER CODE END GPDMA1_Init 1 */
  /* USER CODE BEGIN GPDMA1_Init 2 */

  /* USER CODE END GPDMA1_Init 2 */

}


/* USER CODE BEGIN 4 */
// Functions added 13.01.26 re-added here 14.1.26 -
// ISR Related Added 17.1.26

/* Meteo thread entry function (runs the UART3 interrupt reception) */
/* 27.1.26 not static any more */
void Meteo_Thread_Entry(ULONG thread_input)
{
  (void)thread_input;  // Unused

  /* USER CODE BEGIN METEO_THREAD */
  // Start the initial interrupt reception (single byte)
  if (HAL_UART_Receive_IT(&huart3, &rxByte, 1) != HAL_OK)
  {
    printf("UART3 IT Start Error\r\n");
    Error_Handler();
  }

  while (1)
  {
    // Thread sleeps until interrupt callback wakes it (or use semaphore if needed)
    tx_thread_sleep(50);  // Small sleep to avoid 100% CPU - adjust as needed
  }
  /* USER CODE END METEO_THREAD */
}

/* UART3 RX complete callback (called on each byte) */
// 27.1.26 20:16Hs
// Frame correction 8-2-26 with 16-bit checksum
/* UART3 RX complete callback (called on each byte) */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    if (!frameInProgress)
    {
      /* Look for 'U' 'U' 'U' '$' sequence */
      rxBuffer[rxIndex % 4] = rxByte;
      if (rxIndex >= 3 &&
          rxBuffer[(rxIndex - 3) % 4] == 'U' &&
          rxBuffer[(rxIndex - 2) % 4] == 'U' &&
          rxBuffer[(rxIndex - 1) % 4] == 'U' &&
          rxBuffer[rxIndex % 4] == '$')
      {
        frameInProgress = 1;
        rxBuffer[0] = 'U';
        rxBuffer[1] = 'U';
        rxBuffer[2] = 'U';
        rxBuffer[3] = '$';
        rxIndex = 4;
      }
      else
      {
        rxIndex++;
      }
    }
    else
    {
      /* Collecting frame data */
      if (rxIndex < RX_BUFFER_SIZE - 1)
      {
        rxBuffer[rxIndex++] = rxByte;

        /* Check for end: *QQQ */
        if (rxIndex >= 4 &&
            rxBuffer[rxIndex - 4] == '*' &&
            rxBuffer[rxIndex - 3] == 'Q' &&
            rxBuffer[rxIndex - 2] == 'Q' &&
            rxBuffer[rxIndex - 1] == 'Q')
        {
          rxBuffer[rxIndex] = '\0';

          /* Frame complete - print it */
          printf("\n[METEO Frame] (%d bytes): %s\r\n", rxIndex, rxBuffer);

          /* *** UPDATED: Use new checksum validation *** */
          if (meteo_validate_checksum(rxBuffer))
          {
            printf("[METEO] Checksum OK\n");
            
            // Process frame: display and store
            ProcessMeteoFrame(rxBuffer);           // Display on console
            // 9.2.26- For now comment out - should be queued
            // ProcessMeteoFrameToStream(rxBuffer);   // Store in ITTIA DB stream
          }
          else
          {
            printf("[METEO] Err: Checksum validation failed\n");
            
            // DEBUG: Show calculated vs received checksum
            uint16_t calc_chksum = meteo_calculate_checksum(rxBuffer);
            printf("  Calculated checksum: 0x%04X\n", calc_chksum);
          }

          frameInProgress = 0;
          rxIndex = 0;
        }
      }
      else
      {
        /* Buffer overflow */
        printf("[METEO] Buffer overflow - frame dropped\n");
        frameInProgress = 0;
        rxIndex = 0;
      }
    }

    /* Restart reception */
    HAL_UART_Receive_IT(&huart3, &rxByte, 1);
  }
}

/* UART3 error callback (clear framing errors) */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    uint32_t isr = READ_REG(huart->Instance->ISR);
    printf("[UART ERR: ISR=0x%08lX]\r\n", isr);
    __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF | UART_CLEAR_FEF | UART_CLEAR_NEF);

    // Restart reception on error
    HAL_UART_Receive_IT(&huart3, &rxByte, 1);
  }
}


/**
 * @brief  Parse the meteo frame and display values - 8.2.26
 * @param  frame: null-terminated string starting with "UUU$" ... "*QQQ"
 */
void ProcessMeteoFrame(const char* frame)
{
  uint32_t ts = HAL_GetTick();
  uint32_t temp_adc = 0, baro_adc = 0, wdir = 0, wspeed = 0, volt = 0;
  uint16_t crcc = 0;

  // Format: UUU$ttttt.bbbbb.dddd.sssss.vvv.CRCC*QQQ
  // Parse: skip the CRCC field in scanf
  if (sscanf(frame, "UUU$%5u.%5u.%4u.%5u.%3u.%4hx",
             &temp_adc, &baro_adc, &wdir, &wspeed, &volt, &crcc) >= 5)
  {
    // Convert to engineering units
    float temp_c = temp_adc * 0.01f;        // e.g., 08030 → 80.30°C
    float pressure_hpa = baro_adc * 0.1f;   // e.g., 00327 → 32.7 hPa
    
    // Display on console
    printf("[TS %lu] T=%.2f degC P=%.1fhPa WDir=%u.%u deg WSpeed=%u V=%umV CRC=0x%04X\r\n",
           ts, temp_c, pressure_hpa, wdir/10, wdir%10, wspeed, volt, crcc);

    #ifdef NEW_LCD
    // LCD display code here if needed
    #endif
  }
  else
  {
    printf("[METEO] Parse failed: %s\r\n", frame);
  }
}

// *** REMOVED OLD ChecksumValidate() FUNCTION ***
// 8.2.26 Now using meteo_validate_checksum() from meteo_checksum.c

/* Replaced old function 8-2-26
int ChecksumValidate(const char* frame)
{
  // Find position before *QQQ
  const char* end = strstr(frame, "*QQQ");
  if (!end) return 0;

  // simple 8-bit sum of all chars from UUU$ to before CRCC field
  uint8_t calc = 0;
  const char* p = frame;
  while (p < end)
  {
    calc += (uint8_t)*p++;
  }

  // Extract reported CRCC (assuming it's hex after last . before *)
  uint16_t reported = 0;
  char crcc_str[8];
  if (sscanf(end - 7, "%*[^.] . %hx", &reported) != 1)   // adjust pattern
    return 0;

  return (calc == (reported & 0xFF));   // example - change comparison
}
*/

// 14.1.26 To avoid Linker Error
int _gettimeofday(struct timeval *tv, void *tzvp)
{
    if (tv) {
        tv->tv_sec  = HAL_GetTick() / 1000;           // seconds since boot
        tv->tv_usec = (HAL_GetTick() % 1000) * 1000;  // rough ms to us
    }
    return 0;  // success (dummy)
}


// End added functions


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
