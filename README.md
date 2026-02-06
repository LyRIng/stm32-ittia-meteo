This Project describes the testing sequence for a combination of the STM32H573I-DK and an existing METEO module (Rafael Oliva) using ITTIA-DB-Lite and Analitica software, with emphasis on the data management and storage limitations for the reception and processing of sensor data frames arriving at 1 Hz rate.  Goal is to explore based on ITTIA’s existing examples the interaction of real-time data with the Database and the production on .CSV Files to select optimized Anomaly Detection applications using NanoEdge AI Studio.

Updated 28-01-26 - Correct reading of UART3 Frames / NetX and ThreadX set up correctly.

Updated 1/2 - 2/2 and 3/2 - Runs correctly but HardFault on MX_ITTIA_Init() in app_ittia.c

Updated 6-2-26 Now same Repo with two Branches:

Branch: `master` (Pre-Production - TCP/ITTIA DB with local storage) - Solved HardFault - thanks to Ryan Phillips of ITTIA support.

Process:

git checkout master

Import into STM32CubeIDE:
1. File → Import → Existing Projects into Workspace
2. Right-click project → Convert to C++

Branch: `udp-demo-version` (Demo/Testing)
Status: Working (simplified for demos)  
Tag: `meeting-demo-feb2026`  
Compilation: C (standard)
Once cloned:
 git checkout udp-demo-version