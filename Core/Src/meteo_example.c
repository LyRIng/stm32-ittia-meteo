/**************************************************************************/
/*                                                                        */
/*      METEO Example Implementation                                      */
/*      Integrates UART3 meteorological data with ITTIA DB streaming     */
/*                                                                        */
/**************************************************************************/

#include "meteo_example.h"
#include "meteo_database.h"
#include "meteo_streams.h"

#include <ittia/os/os_wait_time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ITTIA DB stream processing */
#include <ittia/db/db_stream.h>
#include "dbs_error_info.h"

/* Global environment for data streams, shared by all tasks. */
db_stream_environment_t meteo_stream_env = NULL;
int32_t * meteo_instance_id = NULL;

/* Global stream nodes accessible from UART callback */
db_stream_node_t meteo_input_node;
db_stream_node_t meteo_output_node;

/**
 * @brief Initialize METEO example - create stream environment
 */
int meteo_example_init(db_media_driver_t driver, void * driver_info)
{
    dbstatus_t status;
    
    status = db_stream_create_environment(&meteo_stream_env);
    if (DB_FAILED(status)) {
        fprintf(stderr,
            "Cannot initialize METEO stream environment: %s\n",
            dbs_get_error_info(status).description);
        return EXIT_FAILURE;
    }

    printf("METEO example initialized\n");
    return EXIT_SUCCESS;
}

/**
 * @brief Run METEO example - setup streams and wait for UART data
 * This function initializes the stream graph and then returns,
 * allowing the UART ISR to feed data into the stream.
 */
int run_meteo_example(db_media_driver_t driver, void * driver_info)
{
    dbstatus_t status;
    db_stream_graph_t graph;
    
    /* Initialize data streams ----------------------------------------------*/
    status = db_stream_create_graph(&graph, NULL, 0);
    if (DB_FAILED(status)) {
        fprintf(stderr, "Cannot create stream graph: %s\n",
            dbs_get_error_info(status).description);
        return EXIT_FAILURE;
    }

    /* Create meteo_readings4 stream */
    int result = init_meteo_readings_stream(
        meteo_stream_env,
        graph,
        &meteo_input_node,
        &meteo_output_node);
    
    if (result != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    /* Wait for instance ID to be assigned by IDC agent */
    while (NULL == meteo_instance_id || *meteo_instance_id == 0) {
        os_sleep(WAIT_MILLISEC(100));
    }

    printf("METEO example ready - instance_id: %d\n", *meteo_instance_id);
    printf("Waiting for UART3 meteorological data...\n");

    /* Keep the stream graph alive - UART ISR will feed data */
    while (1) {
        os_sleep(WAIT_MILLISEC(1000));
        // Main loop - UART callbacks handle data insertion
    }

    // Cleanup (never reached in this implementation)
    db_stream_free_graph(graph, NULL);
    return EXIT_SUCCESS;
}

/**
 * @brief Process a complete METEO frame from UART3
 * This function is called from your UART callback in main.c
 * 
 * Frame format: UUU$ttttt.bbbbb.dddd.sssss.vvv.CRCC*QQQ
 * - ttttt: Temperature ADC (5 digits)
 * - bbbbb: Barometric pressure ADC (5 digits)  
 * - dddd:  Wind direction in 0.1 degrees (4 digits)
 * - sssss: Wind speed (5 digits)
 * - vvv:   Voltage (3 digits)
 * - CRCC:  Checksum
 */
void ProcessMeteoFrameToStream(const char* frame)
{
    uint32_t temp_adc = 0, baro_adc = 0, wdir = 0, wspeed = 0, volt = 0;
    uint16_t crcc = 0;
    
    /* Parse the METEO frame */
    if (sscanf(frame, "UUU$%5u.%5u.%4u.%5u.%3u.%*[^.].%hu",
               &temp_adc, &baro_adc, &wdir, &wspeed, &volt, &crcc) >= 5)
    {
        /* Create timestamp in microseconds */
        extern uint32_t HAL_GetTick(void);  // From STM32 HAL
        db_timestamp_usec_t timestamp = (db_timestamp_usec_t)HAL_GetTick() * 1000ULL;
        
        /* Convert ADC values to engineering units */
        // TODO: Adjust these conversion factors based on your sensor calibration
        double temperature = temp_adc / 100.0;        // Example: ADC to °C
        double wind_speed = wspeed / 10.0;            // Example: to m/s
        double wind_direction = wdir / 10.0;          // Already in degrees (0.1° resolution)
        
        /* Create meteo reading */
        meteo_readings_row_t meteo = {
            .id = (meteo_instance_id != NULL) ? *meteo_instance_id : 1,
            .ts = timestamp,
            .temperature = temperature,
            .wind_speed = wind_speed,
            .wind_direction = wind_direction
        };
        
        /* Insert into stream */
        dbstatus_t status = put_meteo_readings_stream(meteo_input_node, &meteo);
        
        if (DB_FAILED(status)) {
            fprintf(stderr,
                "Cannot process METEO stream input: %s\n",
                dbs_get_error_info(status).description);
        } else {
            printf("[METEO] T:%.2f°C, WS:%.1fm/s, WD:%.1f°, V:%u\n",
                   temperature, wind_speed, wind_direction, volt);
        }
    }
    else {
        fprintf(stderr, "METEO frame parse error: %s\n", frame);
    }
}
