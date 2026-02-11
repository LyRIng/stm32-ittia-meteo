/**
 * @brief METEO data simulator for testing without real sensor
 * @version 10.02.26 
 * @author R.Oliva
 * @description Generates realistic METEO frames with correct checksums
 *              Console control: Press 'S' to toggle, 'H' for help
 */

#include "meteo_simulator.h"
#include "meteo_checksum.h"
#include "stm32h573i_discovery.h"  // ADD BSP HEADER 10.2.26
#include "tx_api.h"
#include "stm32h5xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations for frame processing
extern void ProcessMeteoFrame(const char* frame);
extern void ProcessMeteoFrameToStream(const char* frame);

// *** USE BSP COM HANDLE ***
extern UART_HandleTypeDef hcom_uart[COM_NBR];  // BSP COM array

static TX_THREAD simulator_thread;
static UCHAR simulator_stack[2048];
static UINT simulator_enabled = 0;

// Simulated sensor ranges
#define SIM_TEMP_MIN     (-1000)    // -10.0°C
#define SIM_TEMP_MAX     (5000)     // 50.0°C
#define SIM_PRESSURE_MIN (9500)     // 950.0 hPa
#define SIM_PRESSURE_MAX (10500)    // 1050.0 hPa
#define SIM_WIND_DIR_MAX (3599)     // 0-359.9 degrees

/**
 * @brief Generate simulated METEO frame with correct checksum
 * @param buffer Output buffer for generated frame
 * @param buf_size Size of output buffer
 */
void meteo_simulator_generate_frame(char *buffer, size_t buf_size)
{
    // Simulate realistic weather data
    static int32_t temp = 2000;      // Start at 20.0°C
    static int32_t pressure = 10132; // Start at 1013.2 hPa
    static uint16_t wind_dir = 0;
    static uint16_t wind_speed = 0;
    static uint16_t voltage = 115;   // 115mV = 1.15V
    
    // Add random variations (realistic changes)
    temp += (rand() % 10) - 5;           // ±0.5°C change
    pressure += (rand() % 5) - 2;        // ±0.2 hPa change
    wind_dir = (wind_dir + (rand() % 50)) % 3600;  // Gradual rotation
    wind_speed = rand() % 40;            // 0-39 units m/s 10/2/26
    voltage = 110 + (rand() % 10);       // 110-119mV
    
    // Clamp to realistic ranges
    if (temp < SIM_TEMP_MIN) temp = SIM_TEMP_MIN;
    if (temp > SIM_TEMP_MAX) temp = SIM_TEMP_MAX;
    if (pressure < SIM_PRESSURE_MIN) pressure = SIM_PRESSURE_MIN;
    if (pressure > SIM_PRESSURE_MAX) pressure = SIM_PRESSURE_MAX;
    if (wind_dir > SIM_WIND_DIR_MAX) wind_dir = SIM_WIND_DIR_MAX;
    
    // Calculate checksum: sum of all 5 values
    uint16_t checksum = (uint16_t)(temp + pressure + wind_dir + wind_speed + voltage);
    
    // Format complete frame with checksum
    // Format: UUU$ttttt.bbbbb.dddd.sssss.vvv.CRCC*QQQ
    snprintf(buffer, buf_size,
             "UUU$%05ld.%05ld.%04u.%05u.%03u.%04x*QQQ",
             (long)temp,
             (long)pressure,
             wind_dir,
             wind_speed,
             voltage,
             checksum);
}

/**
 * @brief Check console for simulator commands (non-blocking)
 */
void meteo_simulator_check_console(void)
{
    uint8_t key;
    
    // Non-blocking receive from VCP/BSP COM1 console
    if (HAL_UART_Receive(&hcom_uart[COM1], &key, 1, 0) == HAL_OK)
    {
        switch(key)
        {
            case 's':
            case 'S':
                // Toggle simulator on/off
                meteo_simulator_toggle();
                break;
                
            case 'h':
            case 'H':
            case '?':
                // Show help
                printf("\n");
                printf("================================================\n");
                printf("         METEO Simulator Commands              \n");
                printf("================================================\n");
                printf("  S - Toggle simulator ON/OFF                  \n");
                printf("  H - Show this help                           \n");
                printf("  R - Reset simulator to defaults              \n");
                printf("  I - Show simulator info/status               \n");
                printf("================================================\n");
                printf("\n");
                break;
                
            case 'r':
            case 'R':
                // Reset simulator (will restart from defaults)
                printf("\n[SIMULATOR] Reset - restarting from default values\n");
                // Note: Static variables will reset on next frame generation
                break;
                
            case 'i':
            case 'I':
                // Show simulator info
                printf("\n");
                printf("=== METEO Simulator Status ===\n");
                printf("  State: %s\n", simulator_enabled ? "ENABLED" : "DISABLED");
                printf("  Mode: Synthetic weather data\n");
                printf("  Temp range: -10.0 to 50.0°C\n");
                printf("  Pressure range: 950.0 to 1050.0 hPa\n");
                printf("  Update rate: 1 frame/second\n");
                printf("===============================\n");
                printf("\n");
                break;
                
            case '\r':
            case '\n':
                // Ignore newlines
                break;
                
            default:
                // Unknown command - show brief hint
                printf("[?] Unknown command '%c' - Press 'H' for help\n", key);
                break;
        }
    }
}

/**
 * @brief Simulator thread - generates frames periodically
 * @param thread_input Unused parameter
 */
void meteo_simulator_thread_entry(ULONG thread_input)
{
    (void)thread_input;
    char sim_frame[64];
    
    printf("\n");
    printf("====================================================\n");
    printf("       METEO Simulator Thread Started              \n");
    printf("====================================================\n");
    printf("  Press 'S' in console to toggle simulator         \n");
    printf("  Press 'H' for help and available commands        \n");
    printf("====================================================\n");
    printf("\n");
    
    while(1)
    {
        // Check for console commands (non-blocking)
        meteo_simulator_check_console();
        
        // Generate frames if simulator is enabled
        if (simulator_enabled)
        {
            // Generate frame with correct checksum
            meteo_simulator_generate_frame(sim_frame, sizeof(sim_frame));
            
            // Display frame (same format as real UART)
            printf("\n[SIMULATOR Frame] (%d bytes): %s\r\n", 
                   (int)strlen(sim_frame), sim_frame);
            
            // Validate our own checksum (should always pass!)
            if (meteo_validate_checksum(sim_frame))
            {
                printf("[SIMULATOR] Checksum OK\n");
                
                // Process frame (display and optionally store)
                ProcessMeteoFrame(sim_frame);
                
                // TODO: Uncomment when queue architecture is ready
                // ProcessMeteoFrameToStream(sim_frame);
            }
            else
            {
                printf("[SIMULATOR] SChecksum FAILED (bug in simulator!)\n");
            }
        }
        
        // Sleep 100ms (check console 10x/sec, generate frames 1x/sec)
        tx_thread_sleep(50);  // 50 ticks = 500ms 10/2/26
    }
}

/**
 * @brief Initialize METEO simulator
 */
void meteo_simulator_init(void)
{
    UINT status;
    
    printf("\n=== Initializing METEO Simulator ===\n");
    printf("Using BSP COM1 for console commands\n");
    
    // Create simulator thread
    status = tx_thread_create(&simulator_thread,
                             "METEO Simulator",
                             meteo_simulator_thread_entry,
                             0,
                             simulator_stack,
                             sizeof(simulator_stack),
                             15,  // Priority (same as other app threads)
                             15,
                             TX_NO_TIME_SLICE,
                             TX_AUTO_START);
    
    if (status != TX_SUCCESS)
    {
        printf("ERROR: Failed to create simulator thread (status=0x%X)\n", status);
    }
    else
    {
        printf("OK Simulator thread created\n");
    }
}

/**
 * @brief Toggle simulator on/off
 */
void meteo_simulator_toggle(void)
{
    simulator_enabled = !simulator_enabled;
    
    if (simulator_enabled)
    {
        printf("\n");
        printf("====================================================\n");
        printf("            METEO Simulator ENABLED                \n");
        printf("====================================================\n");
        printf("  Generating synthetic weather data                \n");
        printf("  Press 'S' to disable and use real UART3 data    \n");
        printf("====================================================\n");
        printf("\n");
    }
    else
    {
        printf("\n");
        printf("====================================================\n");
        printf("           METEO Simulator DISABLED                \n");
        printf("====================================================\n");
        printf("  Using real METEO sensor data from UART3         \n");
        printf("  Press 'S' to enable simulator                    \n");
        printf("====================================================\n");
        printf("\n");
    }
}

/**
 * @brief Check if simulator is enabled
 * @return 1 if enabled, 0 if disabled
 */
UINT meteo_simulator_is_enabled(void)
{
    return simulator_enabled;
}
