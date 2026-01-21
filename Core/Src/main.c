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
// #include "stm32h573i_discovery_lcd.h"   // or your BSP_LCD header 14-1-26 off
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// To avoid linker errors
// using int _gettimeofday(struct timeval *tv, void *tzvp) in USER CODE 4
#include <sys/time.h>
#include <errno.h>

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
uint8_t meteo_thread_stack[1024];  // Small stack for minimal thread

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* Forward declarations 13.01.26 */
void ProcessMeteoFrame(const char* frame);
int ChecksumValidate(const char* frame);   // implement your CRCC logic

// 17.1.26 Thread Entry function
static void Meteo_Thread_Entry(ULONG thread_input);  // Thread entry function

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
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_DCACHE1_Init();
  MX_ICACHE_Init();
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

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  printf("COM - Ok Connection STM32H573! \r\n");

  //printf("Byte pool size: %lu bytes\r\n", byte_pool.tx_byte_pool_memory_size);
  //printf("Free before kernel: %lu bytes\r\n", byte_pool.tx_byte_pool_available_storage);
  /* Create a minimal meteo thread for UART3 reception */
  tx_thread_create(&meteo_thread, "Meteo Thread",
                   Meteo_Thread_Entry, 0,
                   meteo_thread_stack, sizeof(meteo_thread_stack),
                   10, 10, TX_NO_TIME_SLICE, TX_AUTO_START);

  printf("All threads created - starting ThreadX kernel\r\n");
  /* Initialize ThreadX 17.1.26 */
  MX_ThreadX_Init();


    /* Start scheduler */
    /* USER CODE BEGIN PREKERNEL  */
    // Assuming byte_pool is accessible (extern from app_netxduo.c or app_threadx.c)
    //extern TX_BYTE_POOL byte_pool;  // If not already declared in a header
#ifdef TXBYTEPOOLCHECK
    ULONG free_memory = 0;
    if (byte_pool.tx_byte_pool_id == TX_BYTE_POOL_ID)  // Check if initialized
    {
        free_memory = byte_pool.tx_byte_pool_available_storage;
        printf("Before tx_kernel_enter - ThreadX byte pool free: %lu bytes\r\n", free_memory);
    }
    else
    {
        printf("Byte pool not initialized yet\r\n");
    }
#endif

    /* USER CODE END PREKERNEL */


   // tx_kernel_enter();


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

/* USER CODE BEGIN 4 */
// Functions added 13.01.26 re-added here 14.1.26 -
// ISR Related Added 17.1.26

/* Meteo thread entry function (runs the UART3 interrupt reception) */
static void Meteo_Thread_Entry(ULONG thread_input)
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
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART3)
  {
    lastReceptionTick = HAL_GetTick();
    printf("%02X ", rxByte);  // Print hex for debug

    if (!frameInProgress)
    {
      // Sliding window for "UUU$"
      rxBuffer[rxIndex % 4] = rxByte;
      rxIndex++;
      if (rxIndex >= 4 && strncmp(&rxBuffer[rxIndex % 4 - 3], FRAME_START, 4) == 0)
      {
        frameInProgress = 1;
        strcpy(rxBuffer, "UUU$");
        rxIndex = 4;
        printf("\nFrame start detected\r\n");
      }
    }
    else
    {
      rxBuffer[rxIndex++] = rxByte;
      if (rxIndex >= RX_BUFFER_SIZE - 1)
      {
        rxIndex = 0;  // Overflow protection
        frameInProgress = 0;
        printf("\nFrame overflow - discarded\r\n");
      }
      else if (rxIndex >= 7 && strstr(&rxBuffer[rxIndex - 6], FRAME_END) != NULL)
      {
        rxBuffer[rxIndex] = '\0';
        printf("\nFrame complete (%d bytes): %s\r\n", rxIndex, rxBuffer);

        if (ChecksumValidate(rxBuffer))
        {
          ProcessMeteoFrame(rxBuffer);
        }
        else
        {
          printf("Checksum ERROR\r\n");
        }

        frameInProgress = 0;
        rxIndex = 0;
      }
    }

    // Restart reception for next byte
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
 * @brief  Parse the meteo frame and display values
 * @param  frame: null-terminated string starting with "UUU$" ... "*QQQ"
 */
void ProcessMeteoFrame(const char* frame)
{
  uint32_t ts = HAL_GetTick();
  uint32_t temp_adc = 0, baro_adc = 0, wdir = 0, wspeed = 0, volt = 0;
  uint16_t crcc = 0;

  // Example parsing - adjust format exactly to your frame
  // Format: UUU$ttttt.bbbbb.dddd.sssss.vvv.CRCC*QQQ
  if (sscanf(frame, "UUU$%5u.%5u.%4u.%5u.%3u.%*[^.].%hu",
             &temp_adc, &baro_adc, &wdir, &wspeed, &volt, &crcc) >= 5)
  {
    // Display on VCP
    printf("[TS %lu] Temp ADC: %05u | Baro ADC: %05u | WDir: %u (%u.%u deg) | WSpeed: %u | V: %u | CRCC: %hu\r\n",
           ts, temp_adc, baro_adc, wdir, wdir/10, wdir%10, wspeed, volt, crcc);

    // Display on LCD (simple text update - you can improve layout)
    char line[64];

    #ifdef NEW_LCD
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    sprintf(line, "TS: %lu ms", ts);
    BSP_LCD_DisplayStringAt(10, 40, (uint8_t*)line, LEFT_MODE);

    sprintf(line, "Temp ADC: %05u", temp_adc);
    BSP_LCD_DisplayStringAt(10, 70, (uint8_t*)line, LEFT_MODE);

    sprintf(line, "Baro ADC: %05u", baro_adc);
    BSP_LCD_DisplayStringAt(10, 100, (uint8_t*)line, LEFT_MODE);

    sprintf(line, "Wind Dir: %u.%u deg", wdir/10, wdir%10);
    BSP_LCD_DisplayStringAt(10, 130, (uint8_t*)line, LEFT_MODE);

    sprintf(line, "Wind Speed: %u", wspeed);
    BSP_LCD_DisplayStringAt(10, 160, (uint8_t*)line, LEFT_MODE);
    #endif
    // Instead, after parsing success, add simple visual feedback (e.g. green rectangle)
    BSP_LCD_FillRect(0, 200, 200, 40, 40, 0xFF00FF00UL);  // green square = good frame
  }
  else
  {
    printf("Parse failed: %s\r\n", frame);
    #ifdef NEW_LCD
    BSP_LCD_DisplayStringAt(10, 200, (uint8_t*)"Parse ERROR", LEFT_MODE);
    #endif
    // In checksum error:
    BSP_LCD_FillRect(0, 180, 10, 100, 20, 0xFFFF0000UL);  // red bar = error
  }
}

/**
 * @brief  Implement your CRCC checksum validation
 *         Replace this placeholder with your actual algorithm
 * @return 1 = valid, 0 = invalid
 */
int ChecksumValidate(const char* frame)
{
  // Find position before *QQQ
  const char* end = strstr(frame, "*QQQ");
  if (!end) return 0;

  // Example: simple 8-bit sum of all chars from UUU$ to before CRCC field
  // ADJUST THIS TO MATCH YOUR METEO MODULE'S ACTUAL CHECKSUM METHOD
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
