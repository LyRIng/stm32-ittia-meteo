/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team / R.Oliva
  * @brief   NetXDuo applicative file for METEO - rev 16.2.26
  *          Uses ITTIA-DBlite STM32H573 v9.1 Examples structure
  *          Debug info added 17.02.26 in MX_NetXDuo_Init()
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
#include "nxd_dhcp_client.h"
/* USER CODE BEGIN Includes */
#include "nx_stm32_eth_config.h"
#include "meteo_example.h"
#include "db_netxduo_tcp.h"
#include <stdio.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
TX_THREAD      NxAppThread;
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;
TX_SEMAPHORE   DHCPSemaphore;
NX_DHCP        DHCPClient;

/* USER CODE BEGIN PV */
TX_THREAD AppTCPThread;
TX_THREAD AppLinkThread;
ULONG IpAddress;
ULONG NetMask;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID App_Main_Thread_Entry(ULONG thread_input);
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr);

/* USER CODE BEGIN PFP */
static VOID App_TCP_Thread_Entry(ULONG thread_input);
static VOID App_Link_Thread_Entry(ULONG thread_input);

#define PRINT_IP_ADDRESS(addr)             do { \
                                                printf("STM32 %s: %lu.%lu.%lu.%lu \n", #addr, \
                                                (addr >> 24) & 0xff, \
                                                (addr >> 16) & 0xff, \
                                                (addr >> 8) & 0xff, \
                                                addr& 0xff);\
                                           }while(0)
/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  CHAR *pointer;

  /* USER CODE BEGIN 0 */
  // Debug info added 17.02.26
  printf("=== MX_NetXDuo_Init START ===\n");
  printf("Connecting to Ethernet network...\n");
  /* USER CODE END 0 */

  /* Initialize the NetXDuo system */
  printf("Step 1: Calling nx_system_initialize()...\n");
  nx_system_initialize();
  printf("Step 1: OK\n");

  /* Allocate memory for packet pool */
  printf("Step 2: Allocating packet pool memory...\n");
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  printf("Step 2: OK\n");
  /* Create packet pool */
  printf("Step 3: Creating packet pool...\n");
  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, 
                               pointer, NX_APP_PACKET_POOL_SIZE);
  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }
  printf("Step 3: OK\n");
  /* Allocate memory for IP instance */
  printf("Step 4: Allocating IP instance memory...\n");
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  printf("Step 4: OK\n");
  /* Create IP instance */
  printf("Step 5: Creating IP instance...\n");
  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", 
                     NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, 
                     &NxAppPool, nx_stm32_eth_driver,
                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);
  if (ret != NX_SUCCESS)
  {
	  printf("ERROR: IP create failed (0x%X)\n", ret);
	  return NX_NOT_SUCCESSFUL;
  }
  printf("Step 5: OK\n");
  /* Allocate memory for ARP */
  printf("Step 6: Allocating ARP memory...\n");
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  printf("Step 6: OK\n");
  /* Enable ARP */
  printf("Step 6B: Enable ARP ...\n");
  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }
  printf("Step 6B: OK\n");
  /* Enable ICMP */
  printf("Step 7: Enabling ICMP...\n");
  ret = nx_icmp_enable(&NetXDuoEthIpInstance);
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }
  printf("Step 7: OK\n");

  /* USER CODE BEGIN TCP_Protocol_Initialization */
  /* Allocate memory for TCP thread */
  printf("Step 8A: Allocate Mem for TCP thread...\n");
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_TCP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  printf("Step 8A: OK\n");
  /* Create TCP thread */
  printf("Step 8B: Creating TCP thread...\n");
  ret = tx_thread_create(&AppTCPThread, "App TCP Thread", App_TCP_Thread_Entry, 0, 
                         pointer, APP_TCP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, 
                         TX_NO_TIME_SLICE, TX_DONT_START);
  if (ret != TX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }
  printf("Step 8B: OK\n");
  /* USER CODE END TCP_Protocol_Initialization */

  /* Enable TCP */
  printf("Step 9: Enabling TCP...\n");
  ret = nx_tcp_enable(&NetXDuoEthIpInstance);
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }
  printf("Step 9: OK\n");
  /* Enable UDP (required for DHCP) */
  printf("Step 10: Enabling UDP...\n");
  ret = nx_udp_enable(&NetXDuoEthIpInstance);
  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }
  printf("Step 10: OK\n");
  /* Allocate memory for main thread */
  printf("Step 11A: Allocate Mem for main thread...\n");
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  printf("Step 11A: OK\n");
  /* Create main thread */
  printf("Step 11B: Creating main thread...\n");
  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", App_Main_Thread_Entry, 0, 
                         pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, 
                         TX_NO_TIME_SLICE, TX_AUTO_START);
  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
  printf("Step 11B: OK\n");
  /* Create DHCP client */
  printf("Step 12: Creating DHCP client...\n");
  ret = nx_dhcp_create(&DHCPClient, &NetXDuoEthIpInstance, "DHCP Client");
  if (ret != NX_SUCCESS)
  {
    return NX_DHCP_ERROR;
  }
  printf("Step 12: OK\n");
  /* Create DHCP semaphore */
  tx_semaphore_create(&DHCPSemaphore, "DHCP Semaphore", 0);

  /* USER CODE BEGIN MX_NetXDuo_Init */
  /* Allocate memory for Link thread */
  printf("Step 13A: Allocate Mem for Link thread...\n");
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  printf("Step 13A: OK\n");
  /* Create Link monitoring thread */
  printf("Step 13B: Creating Link thread...\n");
  ret = tx_thread_create(&AppLinkThread, "App Link Thread", App_Link_Thread_Entry, 0, 
                         pointer, NX_APP_THREAD_STACK_SIZE,
                         LINK_PRIORITY, LINK_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);
  if (ret != TX_SUCCESS)
  {
    return NX_NOT_ENABLED;
  }
  printf("Step 13B: OK\n");
  /* USER CODE END MX_NetXDuo_Init */
  printf("=== MX_NetXDuo_Init COMPLETE ===\n");
  return ret;
}

/**
  * @brief  IP address change callback
  */
static VOID ip_address_change_notify_callback(NX_IP *ip_instance, VOID *ptr)
{
  /* USER CODE BEGIN ip_address_change_notify_callback */
	printf("\n=== IP CALLBACK TRIGGERED ===\n");
  if (nx_ip_address_get(&NetXDuoEthIpInstance, &IpAddress, &NetMask) != NX_SUCCESS)
  {
	  printf("ERROR: Failed to get IP address\n"); //17.2.26
	  Error_Handler();
  }
  
  printf("IP obtained: ");  // ← ADD
  PRINT_IP_ADDRESS(IpAddress);
  if(IpAddress == NULL_ADDRESS)
  {
    /* Do not release the semaphore */
	printf("IP is 0.0.0.0 - not releasing semaphore\n");
    return;
  }

  /* Release semaphore as soon as IP address is available */
  printf("Releasing DHCP semaphore\n");
  tx_semaphore_put(&DHCPSemaphore);
  printf("Released DHCP semaphore ok\n");  //17.2.26
  /* USER CODE END ip_address_change_notify_callback */
}

/**
  * @brief  Main network thread
  */
static VOID App_Main_Thread_Entry(ULONG thread_input)
{
  UINT ret = NX_SUCCESS;
  printf("\n=== MAIN THREAD STARTED ===\n"); // 17.2.26
  /* Register IP address change callback */
  printf("Registering IP callback...\n");
  ret = nx_ip_address_change_notify(&NetXDuoEthIpInstance, ip_address_change_notify_callback, NULL);
  if (ret != NX_SUCCESS)
  {
    printf("ERROR: IP callback failed (0x%X)\n", ret);
    Error_Handler();
  }
  printf("IP callback registered\n");
  /* Start DHCP client */
  printf("Starting DHCP client...\n");
  ret = nx_dhcp_start(&DHCPClient);
  if (ret != NX_SUCCESS)
  {
	  printf("ERROR: DHCP start failed (0x%X)\n", ret);
	  Error_Handler();
  }
  printf("DHCP client started\n");
  /* Wait until IP address is ready */
  printf("Waiting for DHCP (max 30s)...\n");  // ← ADD
  if(tx_semaphore_get(&DHCPSemaphore, 30 * TX_TIMER_TICKS_PER_SECOND) != TX_SUCCESS)
  {
	  printf("ERROR: DHCP timeout\n");
	  Error_Handler();
  }
  printf("DHCP succeeded!\n");
  /* USER CODE BEGIN Nx_App_Thread_Entry 2 */
  PRINT_IP_ADDRESS(IpAddress);

  /* Network is initialized, start TCP server thread */
  printf("Resuming TCP thread...\n");
  tx_thread_resume(&AppTCPThread);
  printf("TCP thread resumed\n");

  /* Relinquish this thread */
  printf("Main thread exiting\n");
  tx_thread_relinquish();
  /* USER CODE END Nx_App_Thread_Entry 2 */
}

/* USER CODE BEGIN 1 */

/**
  * @brief  TCP thread - runs ITTIA Data Connect agent
  */
static VOID App_TCP_Thread_Entry(ULONG thread_input)
{

    /* Register TCP protocol for ITTIA DB */
    printf("\n=== TCP THREAD STARTED ===\n");  // ← ADD THIS
    printf("Registering TCP protocol...\n");
    DB_NETXDUO_TCP_REGISTER();

    /* Configure TCP options */
    db_netxduo_tcp_options_t options = {
        .size                  = sizeof(db_netxduo_tcp_options_t),
        .netxduo_packet_pool   = &NxAppPool,
        .netxduo_tcp_interface = &NetXDuoEthIpInstance,
        .netxduo_tcp_port      = DEFAULT_PORT  // 5555
    };

    printf("Running ITTIA Data Connect agent on port %d\n", (int)DEFAULT_PORT);
    printf("Waiting for Analitica connection...\n");
    
    /* Run IDC agent (blocks indefinitely) */
    run_meteo_idc_agent("idb+tcp", &options);
    printf("ERROR: IDC agent returned unexpectedly\n");
}

/**
  * @brief  Link monitoring thread
  */
static VOID App_Link_Thread_Entry(ULONG thread_input)
{
  ULONG actual_status;
  UINT linkdown = 0, status;

  while(1)
  {
    /* Check if Ethernet cable is connected */
    status = nx_ip_interface_status_check(&NetXDuoEthIpInstance, 0, NX_IP_LINK_ENABLED,
                                          &actual_status, 10);

    if(status == NX_SUCCESS)
    {
      if(linkdown == 1)
      {
        linkdown = 0;
        printf("The network cable is connected.\n");

        /* Enable PHY Link */
        nx_ip_driver_direct_command(&NetXDuoEthIpInstance, NX_LINK_ENABLE, &actual_status);
        
        /* Check if address is resolved */
        status = nx_ip_interface_status_check(&NetXDuoEthIpInstance, 0, NX_IP_ADDRESS_RESOLVED,
                                              &actual_status, 10);
        if(status == NX_SUCCESS)
        {
          /* Restart DHCP */
          nx_dhcp_stop(&DHCPClient);
          nx_dhcp_reinitialize(&DHCPClient);
          nx_dhcp_start(&DHCPClient);

          /* Wait for new IP address */
          if(tx_semaphore_get(&DHCPSemaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
          {
            Error_Handler();
          }
          PRINT_IP_ADDRESS(IpAddress);
        }
        else
        {
          /* Force DHCP renewal */
          nx_dhcp_client_update_time_remaining(&DHCPClient, 0);
        }
      }
    }
    else
    {
      if(0 == linkdown)
      {
        linkdown = 1;
        printf("The network cable is not connected.\n");
        nx_ip_driver_direct_command(&NetXDuoEthIpInstance, NX_LINK_DISABLE, &actual_status);
      }
    }
    
    /* Sleep 1 second */
    tx_thread_sleep(NX_APP_CABLE_CONNECTION_CHECK_PERIOD);
  }
}

/* USER CODE END 1 */
