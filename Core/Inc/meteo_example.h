/**************************************************************************/
/*                                                                        */
/*      METEO Example - Meteorological data collection and streaming     */
/*      Integrates UART3 frame reception with ITTIA DB                   */
/*                                                                        */
/**************************************************************************/

#ifndef METEO_EXAMPLE_H
#define METEO_EXAMPLE_H

#include <ittia/db/db_index_storage.h>
#include <ittia/db/db_stream.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize METEO example
 * @param driver Media driver (OSPI flash)
 * @param driver_info Driver-specific information
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int meteo_example_init(db_media_driver_t driver, void * driver_info);

/**
 * @brief Run METEO data collection and streaming
 * @param driver Media driver (OSPI flash)
 * @param driver_info Driver-specific information
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int run_meteo_example(db_media_driver_t driver, void * driver_info);

/**
 * @brief Run ITTIA Data Connect agent for Analitica sync
 * @param proto_name Protocol name ("idb+tcp")
 * @param proto_param Protocol parameters (TCP/IP config)
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int run_meteo_idc_agent(const char * proto_name, void * proto_param);

//3.2.26 - commented out - 8.2.26 reinserted declaration
void ProcessMeteoFrameToStream(const char* frame);

/* Global stream environment shared by METEO threads */
extern db_stream_environment_t meteo_stream_env;
extern int32_t * meteo_instance_id;

/* Stream nodes for METEO data processing */
extern db_stream_node_t meteo_input_node;
extern db_stream_node_t meteo_output_node;

/* METEO configuration */
#define METEO_UPDATE_RATE_HZ    1  // 1 Hz update rate to Analitica

#ifdef __cplusplus
}
#endif

#endif // METEO_EXAMPLE_H
