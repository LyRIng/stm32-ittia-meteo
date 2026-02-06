/**
  ******************************************************************************
  * @file    meteo_network_test.c - 4/2/26
  * @brief   Send METEO data via UDP - bypass ITTIA DB for network testing
  ******************************************************************************
  */

#include "meteo_network_test.h"
#include "nx_api.h"
#include "tx_api.h"
#include <stdio.h>
#include <string.h>

/* Network configuration VM is .125 */
#define ANALITICA_IP_ADDR   IP_ADDRESS(192,168,3,125)
#define ANALITICA_PORT      5555

/* Global variables */
// extern NX_IP nx_ip; changed 4/2/26
extern NX_IP NetXDuoEthIpInstance;
#define nx_ip NetXDuoEthIpInstance  // Alias for convenience
static NX_UDP_SOCKET meteo_udp_socket;
static UINT socket_created = 0;

/**
  * @brief  Initialize METEO network test (UDP sender)
  * @retval None
  */
void meteo_network_test_init(void)
{
    UINT status;
    
    if (socket_created) {
        printf("METEO network test already initialized\n");
        return;
    }
    
    printf("\n=== Initializing METEO Network Test ===\n");
    
    /* Create UDP socket */
    status = nx_udp_socket_create(&nx_ip, 
                                   &meteo_udp_socket,
                                   "METEO UDP Socket",
                                   NX_IP_NORMAL, 
                                   NX_FRAGMENT_OKAY,
                                   NX_IP_TIME_TO_LIVE, 
                                   5);
    
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to create UDP socket (status=0x%x)\n", status);
        return;
    }
    
    /* Bind to any available port */
    status = nx_udp_socket_bind(&meteo_udp_socket, 
                                NX_ANY_PORT, 
                                TX_WAIT_FOREVER);
    
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to bind UDP socket (status=0x%x)\n", status);
        nx_udp_socket_delete(&meteo_udp_socket);
        return;
    }
    
    socket_created = 1;
    
    printf("✓ UDP socket created and bound\n");
    printf("✓ Ready to send to Analitica: 192.168.3.165:%d\n", ANALITICA_PORT);
    printf("=====================================\n\n");
}

/**
  * @brief  Send METEO data via UDP to Analitica
  * @param  temp_c: Temperature in Celsius
  * @param  pressure_hpa: Pressure in hPa
  * @param  wind_dir: Wind direction in degrees * 10
  * @param  wind_speed: Wind speed
  * @param  voltage: Supply voltage in mV
  * @retval None
  */
void send_meteo_data_udp(float temp_c, float pressure_hpa,
                         uint16_t wind_dir, uint16_t wind_speed,
                         uint16_t voltage)
{
    NX_PACKET *packet;
    UINT status;
    char json_buffer[256];
    UINT json_length;
    ULONG timestamp;
    
    if (!socket_created) {
        printf("WARNING: UDP socket not initialized\n");
        return;
    }
    
    /* Get timestamp */
    timestamp = tx_time_get();
    
    /* Format data as JSON */
    json_length = sprintf(json_buffer,
        "{"
        "\"timestamp\":%lu,"
        "\"sensor\":\"METEO\","
        "\"temperature\":%.2f,"
        "\"pressure\":%.2f,"
        "\"wind_direction\":%.1f,"
        "\"wind_speed\":%u,"
        "\"voltage\":%.2f"
        "}\n",
        timestamp,
        temp_c,
        pressure_hpa,
        wind_dir / 10.0f,  // Convert to degrees
        wind_speed,
        voltage / 1000.0f   // Convert mV to V
    );
    
    /* Allocate packet from pool */
    status = nx_packet_allocate(&nx_ip.nx_ip_default_packet_pool,
                                 &packet,
                                 NX_UDP_PACKET,
                                 TX_WAIT_FOREVER);
    
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to allocate packet (status=0x%x)\n", status);
        return;
    }
    
    /* Append JSON data to packet */
    status = nx_packet_data_append(packet,
                                    json_buffer,
                                    json_length,
                                    &nx_ip.nx_ip_default_packet_pool,
                                    TX_WAIT_FOREVER);
    
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to append data (status=0x%x)\n", status);
        nx_packet_release(packet);
        return;
    }
    
    /* Send UDP packet to Analitica */
    status = nx_udp_socket_send(&meteo_udp_socket,
                                 packet,
                                 ANALITICA_IP_ADDR,
                                 ANALITICA_PORT);
    
    if (status != NX_SUCCESS)
    {
        printf("ERROR: Failed to send UDP packet (status=0x%x)\n", status);
        nx_packet_release(packet);
        return;
    }
    
    /* Success - print confirmation */
    printf("[UDP] Sent: T=%.2f°C P=%.2fhPa WD=%.1f° WS=%u V=%.2fV\n",
           temp_c, pressure_hpa, wind_dir/10.0f, wind_speed, voltage/1000.0f);
}
