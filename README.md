## STM32H573I-DK Project with ITTIA-DB-Lite and METEO communication

This Project describes the testing sequence for a combination of the STM32H573I-DK and an existing METEO module (Rafael Oliva) using ITTIA-DB-Lite and Analitica software, with emphasis on the data management and storage limitations for the reception and processing of sensor data frames arriving at 1 Hz rate.  Goal is to explore based on ITTIA’s existing examples the interaction of real-time data with the Database and the production on .CSV Files to select optimized Anomaly Detection applications using NanoEdge AI Studio.


**Update sequence:**

**Updated 28-01-26** - Correct reading of UART3 Frames / NetX and ThreadX set up correctly.

**Updated 1/2 - 2/2 and 3/2** - Runs correctly but HardFault on MX_ITTIA_Init() in app_ittia.c

**Updated 6-2-26** Now same Repo with two Branches:

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
 
**Updated 10-2-26 With METEO Simulator** 

For testing without physical METEO hardware:

**Commands:**
- `S` - Toggle simulator ON/OFF
- `H` - Show help menu
- `I` - Show simulator status
- `R` - Reset to defaults

**Features:**
- Realistic meteo data generation
- Valid checksum calculation
- Gradual changes (temperature drift, wind rotation)
- 1 frame per second
- Non-blocking console control

**Usage:**

Branch: `master` (Pre-Production - TCP/ITTIA DB with local storage)
1. Flash firmware to STM32H573I-DK
2. Open serial terminal (115200 baud)
3. Press 'S' to enable simulator
4. Press 'S' again to use real METEO sensor

**Demo Mode:**
Usable for client presentations without requiring METEO hardware.

**Updated 17-2-26 With IDC connection via Ethernet to Analitica** 
Options: 'S' to enable/disable Simulator

DHCP Enabled - Will obtain IP
IP obtained: STM32 IpAddress: 192.168.3.165
Runs ITTIA Data Connect agent on port 5555

## 06.03.26 Corrected sync problems with Analitica (Usable version)

Now operable - See Commit reports on:
ProcessMeteoFrameToStream() repair, meteo_simulator.c
Changed IDC data model instance_id from -1 to 1 in meteo_idc_agent.c
Implemented incrementing reading IDs in meteo_example.c
Updated IDC sync interval to 2 seconds (2,000,000 microseconds)
Tested with Analitica on Ubuntu VM, Tested with 1000+ measurements
Real METEO Data: Pot adjusted to Wind direction 227.7° confirmed in Analitica

## System Architecture
```
┌─────────────────┐
│  METEOModule L&R│ (UART3 @ 38400 baud)
│  or Simulator   │
└────────┬────────┘
         │ Parse frames
         ▼
┌─────────────────┐
│  ThreadX Queue  │ (10 messages, 64 bytes each)
└────────┬────────┘
         │ Process
         ▼
┌─────────────────┐
│  ITTIA DB Lite  │ (Real-time stream: meteo_readings4)
│  Stream Engine  │ (5 columns: id, ts, temp, wind_speed, wind_dir)
└────────┬────────┘
         │ Sync every 5s
         ▼
┌─────────────────┐
│  IDC Agent      │ (TCP port 5555)
│  NetX Duo Stack │
└────────┬────────┘
         │ Ethernet
         ▼
┌─────────────────┐
│  ITTIA Analitica│ (Ubuntu 22.04)
│  Dashboard      │ (http://127.0.0.1:8080)
└─────────────────┘
```

## Quick Start

### 1. Flash the Board
```bash
# Build in STM32CubeIDE
# Flash to STM32H573I-DK
# Connect Ethernet cable
# Open serial terminal (115200 baud)
```

### 2. Start Analitica (Ubuntu)
```bash
cd ~/opt/ittiadb-8.11.0/bin
./ittiadb analitica --config=~/ittiadb-analitica.yaml --verbose
```

### 3. Enable Simulator or Connect Sensor
```
Press 'S' in serial terminal to toggle simulator
OR
Connect METEO sensor to UART3 (CN7 pins)
```

### 4. View Data in Analitica
```
Open browser: http://127.0.0.1:8080
Navigate: Data Storage → disk_database → meteo_readings4
Query: SELECT * FROM meteo_readings4
```

## Configuration

### Analitica YAML (`~/ittiadb-analitica.yaml`)
```yaml
ittiadb-analitica:
  default:
    url: http://127.0.0.1:8080
    database:
      disk_database:
        location: disk_database.ittiadb
        access: write
        open_mode: overwrite
    data-model:
      meteo_weather_station:
        database: disk_database
        tables:
          meteo_readings4:
            columns:
              id: {type: int32, nullable: false}
              ts: {type: timestamp, nullable: false}
              temperature: {type: float64, nullable: false}
              wind_speed: {type: float64, nullable: false}
              wind_direction: {type: float64, nullable: false}
            primary-key: [id]
    ittia-data-connect:
      device1:
        agent-uri: idb+tcp://192.168.3.165:5555/
        database: disk_database
        data-model: meteo_weather_station
```

### Network Settings
- **Board IP:** Auto-assigned via DHCP (typically 192.168.3.165)
- **IDC Port:** 5555
- **Sync Interval:** 2 seconds
- **Protocol:** TCP/IP

## Data Format

### METEO Frame (UART3)
```
UUU$TTTTT.PPPPP.DDDD.SSSSS.VVV.CCCC*QQQ
     │      │     │     │     │    │
     │      │     │     │     │    └─ CRC16
     │      │     │     │     └────── Battery voltage (mV)
     │      │     │     └──────────── Wind speed (0.1 m/s)
     │      │     └────────────────── Wind direction (0.1°)
     │      └──────────────────────── Pressure (0.1 hPa)
     └─────────────────────────────── Temperature (0.01°C)
```

### Database Schema
```sql
CREATE TABLE meteo_readings4 (
    id              INT32 PRIMARY KEY,
    ts              TIMESTAMP NOT NULL,
    temperature     FLOAT64 NOT NULL,
    wind_speed      FLOAT64 NOT NULL,
    wind_direction  FLOAT64 NOT NULL
);
```

## Troubleshooting

### No data in Analitica
1. Check board terminal shows: `[DB] Stored: T=XX.XX degC...`
2. Verify network: `ping 192.168.3.165`
3. Test connection: `telnet 192.168.3.165 5555`
4. Check Analitica terminal for errors

### Connection refused
- Ensure Analitica YAML has correct IP address
- Verify board obtained IP via DHCP
- Check firewall allows port 5555

### Data stops flowing
- Press 'S' to re-enable simulator
- Check METEO sensor connection (UART3)
- Restart Analitica if connection lost

## Console Commands

- **'S'** - Toggle simulator on/off
- **'H'** - Show help and available commands

## License

See LICENSE file for details.