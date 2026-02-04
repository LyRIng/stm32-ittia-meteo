/**************************************************************************/
/*                                                                        */
/*      METEO IDC Agent - ITTIA Data Connect for Analitica sync          */
/*      Synchronizes METEO data to Ubuntu VM via TCP/Ethernet            */
/*                                                                        */
/**************************************************************************/

#include "meteo_example.h"
#include "main.h"

#include <ittia/os/os_wait_time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ITTIA DB stream processing */
#include <ittia/db/db_stream.h>
/* ITTIA Data Connect agent */
#include <ittia/idc/idc_agent.h>

#define kMeteoDataModelName "meteo_weather_station"

/**
 * @brief Configure relations to synchronize with ITTIA Analitica
 * 
 * This maps local streams to Analitica database tables.
 * The relation_name MUST match the table name in Analitica.
 */
static const idc_synchronized_relation_t meteo_relation_array[] = {
    {
        .model_name = kMeteoDataModelName,
        .relation_name = "meteo_readings4",      // MUST match Analitica table!
        .relation_type = kRealTimeView,
        .update_interval = 1000000 / METEO_UPDATE_RATE_HZ,  // microseconds
        .max_key_cardinality = 1,
    },
};

/**
 * @brief Data model instance configuration
 * The instance_id will be assigned by Analitica on first connection
 */
static idc_data_model_instance_t meteo_instance = {
    -1,  // Will be assigned by Analitica
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

    printf("Starting METEO IDC agent...\n");
    printf("Protocol: %s\n", proto_name);
    printf("Synchronizing to Analitica...\n");

    while (1)
    {
        const size_t relation_count = DB_ARRAY_DIM(meteo_relation_array);
        dbstatus_t status;

        /* Run the IDC agent - this blocks until connection is lost */
        status = idc_run_agent(
            meteo_stream_env,
            meteo_relation_array,
            relation_count,
            &meteo_instance,
            1,  // Number of instances
            proto_name,
            proto_param);

        if (DB_FAILED(status)) {
            printf("IDC agent error: %d - Retrying in 5 seconds...\n", status);
            os_sleep(WAIT_MILLISEC(5000));
        }
        else {
            printf("IDC agent stopped - Reconnecting...\n");
            os_sleep(WAIT_MILLISEC(1000));
        }
    }

    return EXIT_SUCCESS;
}
