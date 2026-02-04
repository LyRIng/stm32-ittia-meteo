This Project describes the testing sequence for a combination of the STM32H573I-DK and an existing METEO module (Rafael Oliva) using ITTIA-DB-Lite and Analitica software, with emphasis on the data management and storage limitations for the reception and processing of sensor data frames arriving at 1 Hz rate. Goal is to explore based on ITTIA’s existing examples the interaction of real-time data with the Database and the production on .CSV Files to select optimized Anomaly Detection applications using NanoEdge AI Studio.

Updated 28-01-26 - Correct reading of UART3 Frames / NetX and ThreadX set up correctly.

Updated 1/2 - 2/2 and 3/2 - Runs correctly but HardFAult on MX_ITTIA_Init() in app_ittia.c
