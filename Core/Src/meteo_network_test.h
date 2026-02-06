/**
  ******************************************************************************
  * @file    meteo_network_test.h
  * @brief   METEO network test header - bypass ITTIA DB 4/2/26
  ******************************************************************************
  */

#ifndef METEO_NETWORK_TEST_H
#define METEO_NETWORK_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* Function prototypes */
void meteo_network_test_init(void);
void send_meteo_data_udp(float temp_c, float pressure_hpa, 
                         uint16_t wind_dir, uint16_t wind_speed, 
                         uint16_t voltage);

#ifdef __cplusplus
}
#endif

#endif /* METEO_NETWORK_TEST_H */