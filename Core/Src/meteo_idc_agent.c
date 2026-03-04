/**************************************************************************/
/*                                                                        */
/*      METEO IDC Agent - ITTIA Data Connect for Analitica sync           */
/*      Synchronizes METEO data to Ubuntu VM via TCP/Ethernet             */
/*      Rev. 17.2.26 - Added Debug info - connection errors               */
/*      Rev2. 6.3.26 - instance_id in line 57 (from -1 to 1)              */
/*                                                                        */
/**************************************************************************/

#include "meteo_example.h"
#include "main.h"

#include <ittia/os/os_wait_time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h> // 18.2.26 For size_t printing

/* ITTIA DB stream processing */
#include <ittia/db/db_stream.h>
/* ITTIA Data Connect agent */
#include <ittia/idc/idc_agent.h>
// 17.2.26 - Add Error information headers
#include <ittia/db/db_error.h>
#include "dbs_error_info.h"
// Pre-Flight schecks 17.2.26
#include "db_netxduo_tcp.h"     // For db_netxduo_tcp_options_t
#include "nx_api.h"             // For nx_ip_address_get(), NX_SUCCESS

#define kMeteoDataModelName "meteo_weather_station"

/**
 * @brief Configure relations to synchronize with ITTIA Analitica
 * 
 * This maps local streams to Analitica database tables.
 * The relation_name MUST match the table name in Analitica.
 * 4.3.26 Added missing db_timestamp_usec_t created_time; in meteo_relation_array[]
 * -- this is defined in idc_agent.h - line 48
 * 6.3.26 Update .update_interval from 1000000 (1s) to 2000000 (2s)
 *   and .max_key_cardinality = 1000,    [ 6.3.26 Allow up to 1000 rows (was 1)]
 */
static const idc_synchronized_relation_t meteo_relation_array[] = {
    {
        .model_name = kMeteoDataModelName,
        .relation_name = "meteo_readings4",      // MUST match Analitica table!
		.created_time = 0,                       // Added 4.3.26  Was missing
        .relation_type = kRealTimeView,
        .update_interval = 2000000 / METEO_UPDATE_RATE_HZ,  // microseconds upd 6.3.26
        .max_key_cardinality = 1000,             // 6.3.26 Allow up to 1000 rows (was 1)
    },
};

/**
 * @brief Data model instance configuration
 * The instance_id will be assigned by Analitica on first connection
 * 6.3.26 - Comparing with ITTIA_NorX SensorFusion, id_ hardcoded to 1 (not -1)
 */
static idc_data_model_instance_t meteo_instance = {
    1,  // 6.3.26 - Change to 1 as in examples
    kMeteoDataModelName,
    "STM32H573 Meteorological Weather Station",
};

/**
 * @brief Run ITTIA Data Connect agent
 * 
 * This function runs continuously, synchronizing local METEO data
 * to ITTIA Analitica running on Ubuntu VM via TCP/Ethernet.
 * 
 * @param proto_name Protocol name (should be "idb+tcp")
 * @param proto_param Protocol parameters (db_netxduo_tcp_options_t*)
 * @return EXIT_SUCCESS (or loops forever on error with retry)
 */
int run_meteo_idc_agent(const char * proto_name, void * proto_param)
{
    meteo_instance_id = &meteo_instance.instance_id;
	
    printf("\n=== PRE-FLIGHT CHECKS ===\n"); // Added 17.2.26
	printf("1.IDC Agent: meteo_stream_env = 0x%p\n", meteo_stream_env);  // Added 17.2.26

    /* Check stream env is initialized */
    if (meteo_stream_env == NULL)
    {
        printf("ERROR: meteo_stream_env is NULL! Streams not initialized yet.\n");
        printf("Waiting for streams to initialize...\n");
        
        /* Wait for stream env to be initialized */
        while (meteo_stream_env == NULL)
        {
            os_sleep(WAIT_MILLISEC(500));
        }
        printf("Stream env ready: 0x%p\n", meteo_stream_env);
    }

    /* Check 2: Relations -upd to avoid %z (not supported STM32) for size_t 3-3-26 */

    //     const size_t relation_count = DB_ARRAY_DIM(meteo_relation_array);
	//     printf("2. Relation count: %zu\n", relation_count);
	//     if (relation_count == 0) {
	//         printf("   ERROR: No relations defined!\n");
	//         return EXIT_FAILURE;
	//     }
	//     printf("   OK: %zu relation(s) defined\n", relation_count);
    const size_t relation_count = DB_ARRAY_DIM(meteo_relation_array);
    printf("2. Relation count: %lu\n", (unsigned long)relation_count);  // ← Use %lu
    if (relation_count == 0) {
         printf("   ERROR: No relations defined!\n");
         return EXIT_FAILURE;
    }
    printf("   OK: %lu relation(s) defined\n", (unsigned long)relation_count);

    // *** For DEBUG 3.3.26 ***
    printf("   DEBUG: relation_array address: %p\n", (void*)meteo_relation_array);
    printf("   DEBUG: array size: %lu bytes\n", (unsigned long)sizeof(meteo_relation_array));
    printf("   DEBUG: element size: %lu bytes\n", (unsigned long)sizeof(idc_synchronized_relation_t));

    /* Check 3: Instance */
     printf("3. Data model instance:\n");
     printf("   - ID: %ld\n", (long)meteo_instance.instance_id);  // ← Use %ld for int32_t
     printf("   - Model: %s\n", meteo_instance.data_model_name);


     /* Check 4: Protocol parameters */
     printf("4. Protocol: %s\n", proto_name);
     if (proto_param == NULL) {
         printf("   ERROR: proto_param is NULL!\n");
         return EXIT_FAILURE;
     }

     db_netxduo_tcp_options_t* opts = (db_netxduo_tcp_options_t*)proto_param;
     printf("   - Port: %d\n", (int)opts->netxduo_tcp_port);
     printf("   - Packet pool: 0x%p\n", (void*)opts->netxduo_packet_pool);
     printf("   - IP instance: 0x%p\n", (void*)opts->netxduo_tcp_interface);

     /* Check 5: Network is ready */
     ULONG ip_address, network_mask;
     if (nx_ip_address_get(opts->netxduo_tcp_interface, &ip_address, &network_mask) == NX_SUCCESS) {
         printf("5. Network status:\n");
         printf("   - IP: %lu.%lu.%lu.%lu\n",
                (ip_address >> 24) & 0xFF,
                (ip_address >> 16) & 0xFF,
                (ip_address >> 8) & 0xFF,
                ip_address & 0xFF);
         printf("   OK: Network ready\n");
     } else {
         printf("5. Network status: ERROR - No IP address!\n");
         return EXIT_FAILURE;
     }

    printf("=========================\n\n");




    printf("Starting METEO IDC agent...\n");
    printf("Protocol: %s\n", proto_name);
    printf("Synchronizing to Analitica...\n");

    while (1)
    {
        const size_t relation_count = DB_ARRAY_DIM(meteo_relation_array);
        dbstatus_t status;
		
		printf("IDC: Calling idc_run_agent()...\n"); // Debug 17.2.26

        /* Run the IDC agent - this blocks until connection is lost */
        status = idc_run_agent(
            meteo_stream_env,
            meteo_relation_array,
            relation_count,
            &meteo_instance,
            1,  // Number of instances
            proto_name,
            proto_param);

   	    /* Get err info - Added debug information - status 17.2.26 */
   	    dbs_error_info_t err_info = dbs_get_error_info(status); // get result
        printf("\n=== IDC AGENT RETURNED ===\n");
        printf("Status: %d (0x%X)\n", status, (unsigned int)status);
        printf("Name: %s\n", err_info.name);
        printf("Description: %s\n", err_info.description);

        if (DB_FAILED(status)) {   // Added debug information - status 17.2.26

             printf("Fail: IDC agent error: %d (0x%X) %s \n",
                   status, (unsigned int)status, err_info.description);
            
            /* Decode error -57 */
            if (status == -57) {
                printf(" Error -57: DB_ECONNREFUSED\n");
                printf("- Socket not connected: IDC couldn't create/bind socket\n");
                printf("- Check: DB_NETXDUO_TCP_REGISTER() was called\n");
                printf("- Check: Network stack is fully initialized\n");
                printf("- Check: Port 5555 not already in use\n");
            }
            else if (status == -318) {
                printf("  Error: DB_EIPCCONNECT - Connection dropped\n");
            }
            printf("  Retrying in 5 seconds...\n");
	        os_sleep(WAIT_MILLISEC(5000));
        }
        else {
            printf("IDC agent stopped (status=%d) - Reconnecting in 1s...\n", status);
            os_sleep(WAIT_MILLISEC(1000));
        }
    }

    return EXIT_SUCCESS;
}
