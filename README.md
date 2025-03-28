# ESP8266 Power Monitoring with AP Mode

## Overview
This project is an ESP8266-based **Power Monitoring System** that reads voltage, current, power, energy, frequency, and power factor using the **PZEM-004T-100A** sensor. It runs as a **standalone access point (AP Mode)** and serves a web interface where real-time power data can be viewed.

## Features
✅ **Wi-Fi Access Point (AP Mode)** – No need for an external router. The ESP8266 hosts its own Wi-Fi network.
✅ **Real-time Power Monitoring** – Displays voltage, current, power, energy, frequency, and power factor.
✅ **Web Dashboard** – Responsive HTML page with live updates using AJAX.
✅ **Cost Calculation** – Converts power consumption into a peso value (₱) based on energy usage.
✅ **Relay Control** – Supports turning SSR relays ON/OFF via Telegram commands.
✅ **Copyright Footer** – Includes "© 2025 Dave | ESP8266 Power Monitor" on the web interface.

## Components Required
- **ESP8266 (NodeMCU, Wemos D1 Mini, or similar)**
- **PZEM-004T-100A Power Meter Sensor**
- **Solid State Relays (SSR)**
- **SIM800L Module (Optional, for GSM-based control)**
- **Power Supply (5V for ESP8266, 12V for PZEM-004T)**

## Circuit Connection
| ESP8266 Pin  | PZEM-004T Pin |
|-------------|--------------|
| 5V          | VCC          |
| GND         | GND          |
| RX (D7)     | TX          |
| TX (D6)     | RX          |

## Installation Guide
### 1️⃣ Flash the ESP8266
Upload the firmware using Arduino IDE or PlatformIO.
Ensure you have the following libraries installed:
- `ESP8266WiFi`
- `ESPAsyncWebServer`
- `ArduinoJson`
- `PZEM004Tv30`
- `EEPROM`

### 2️⃣ Connect to ESP8266 Wi-Fi
- After flashing, the ESP8266 creates a Wi-Fi network called `ESP8266_PowerMonitor`.
- Connect to this network and open a browser.
- Visit `http://192.168.4.1` to access the web dashboard.

## Web Dashboard
- **Live Power Data**: Displays voltage, current, power, energy, frequency, power factor, and cost.
- **Relay Control**: Sends Telegram commands to control SSR relays.

## API Endpoints
| Method | Endpoint    | Description        |
|--------|------------|--------------------|
| GET    | `/data`    | Returns power data |
| POST   | `/relay`   | Control SSR relays |

## Example API Response
```json
{
  "voltage": 220.5,
  "current": 1.23,
  "power": 271.2,
  "energy": 10.5,
  "frequency": 60,
  "pf": 0.95,
  "cost": 50.25
}
```

## Notes
- Default AP Mode IP: `192.168.4.1`
- Modify the **kWh cost rate** in the code to match your local electricity rates.

## License
📜 **MIT License** – Free to use and modify.

## Author
👤 **Dave** – 2025 | ESP8266 Power Monitor

