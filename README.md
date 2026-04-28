# ESP32 Robotics Rover: 3-Mode Control

An intelligent, WiFi-controlled robotics rover powered by the ESP32. This rover features an interactive web dashboard with three distinct operation modes: Manual Control, Obstacle Avoidance, and Smart Explorer.

---

## 📸 Project Showcase

Here are some photos of the rover:

<div style="display: flex; flex-wrap: wrap; gap: 10px;">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.16%20PM.jpeg" width="30%" alt="Rover Photo 1">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.17%20PM.jpeg" width="30%" alt="Rover Photo 2">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.18%20PM.jpeg" width="30%" alt="Rover Photo 3">
</div>
<div style="display: flex; flex-wrap: wrap; gap: 10px; margin-top: 10px;">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.17%20PM%20(1).jpeg" width="46%" alt="Rover Photo 4">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.17%20PM%20(2).jpeg" width="46%" alt="Rover Photo 5">
</div>

---

## ✨ Features

- **Built-in Web Server UI**: A sleek, CSS-styled responsive dashboard hosted directly on the ESP32.
- **Mode 1 - Manual Control**: A D-Pad interface for full directional control (Forward, Reverse, Left, Right) plus a "Spin Combo" function.
- **Mode 2 - Obstacle Avoidance**: Autonomous mode where the rover navigates independently, stops before obstacles (<25cm), and alternates turning direction.
- **Mode 3 - Smart Explorer**: An advanced autonomous mode that scales motor speed proportionally based on distance. It includes emergency evasion maneuvers, "slow mode" near obstacles, and "turbo mode" on clear paths.
- **Live Distance Tracking**: HC-SR04 distance readings are streamed to the web dashboard in real-time.

---

## 🧰 Hardware & Connections

### Parts Required
- 1× ESP32 Development Board
- 1× L298N Motor Driver
- 1× HC-SR04 Ultrasonic Distance Sensor
- DC Motors & Robot Chassis
- 12V Battery Power Source

### Wiring Guide

Ensure correct power setup:
- **Battery** → L298N 12V
- **L298N GND** → ESP32 GND *[CRITICAL: Common Ground]*
- **Motors** → L298N OUT1, OUT2, OUT3, OUT4
- **HC-SR04 Power** → ESP32 VIN (5V) & GND

#### ESP32 Pin Mapping

| Component | Pin Function | ESP32 GPIO |
|-----------|--------------|------------|
| L298N     | IN1          | GPIO 26    |
| L298N     | IN2          | GPIO 27    |
| L298N     | IN3          | GPIO 14    |
| L298N     | IN4          | GPIO 12    |
| L298N     | ENA          | GPIO 25    |
| L298N     | ENB          | GPIO 33    |
| HC-SR04   | TRIG         | GPIO 13    |
| HC-SR04   | ECHO         | GPIO 34 (Input-only, safe)|

---

## 🛠️ Arduino IDE Setup

This project uses the Arduino IDE. Please follow these steps carefully to ensure the ESP32 handles the web server and hardware correctly.

### 1. Install ESP32 Board Support
1. Open Arduino IDE. Go to **File > Preferences**.
2. Add the following URL to *Additional Boards Manager URLs*:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Go to **Tools > Board > Boards Manager...**
4. Search for `esp32`.
5. ⚠️ **CRITICAL: VERSION SELECTION** ⚠️ 
   Select version **`2.0.17`** from the dropdown menu and click Install. *This specific version is required for optimal stability of the Wi-Fi and PWM libraries used in this project.*

### 2. Configure Your Board
- Go to **Tools > Board** and select your specific ESP32 board (e.g., *DOIT ESP32 DEVKIT V1* or *ESP32 Dev Module*).
- Ensure the upload speed is set to `115200`.

### 3. Flash the Code
1. Open [`CODE/esp32_rover.ino`](CODE/esp32_rover.ino) in your Arduino IDE.
2. Update the Wi-Fi credentials in the script to match your local network or desired hotspot:
   ```cpp
   const char* ssid     = "YOUR_SSID";
   const char* password = "YOUR_PASSWORD";
   ```
3. Connect your ESP32 to your PC via USB.
4. Select the corresponding COM port in **Tools > Port**.
5. Click **Upload**.

---

## 🚀 Running the Rover

1. Once uploaded, open the **Serial Monitor** at baud rate `115200`.
2. Press the **EN/RST** button on the ESP32.
3. Wait for the `IP: xxx.xxx.xxx.xxx` to appear in the serial monitor.
4. Connect your phone or computer to the same Wi-Fi network.
5. Enter the given IP address into a web browser.
6. Use the web dashboard and enjoy the drive!

---
*Developed for Robotics Project.*
