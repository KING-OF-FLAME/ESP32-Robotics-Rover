<div align="center">

# 🤖 ESP32 Robotics Rover: 3-Mode Control Hub

[![ESP32](https://img.shields.io/badge/ESP32-v2.0.17-blue?style=for-the-badge&logo=espressif)](https://www.espressif.com/en/products/socs/esp32)
[![Arduino IDE](https://img.shields.io/badge/Arduino_IDE-00979D?style=for-the-badge&logo=arduino&logoColor=white)](https://www.arduino.cc/en/software)
[![C++](https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)]()

An intelligent, WiFi-controlled robotics rover powered by the ESP32. Featuring an interactive web dashboard with **Manual Control**, **Obstacle Avoidance**, and a **Smart Explorer** mode.

<br>

[![LinkedIn](https://img.shields.io/badge/LinkedIn-Yash_Developer-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/yash-developer/)
[![Instagram](https://img.shields.io/badge/Instagram-@yash.developer-E4405F?style=for-the-badge&logo=instagram&logoColor=white)](https://www.instagram.com/yash.developer)

</div>

<hr>

## 📸 Project Showcase

<p align="center">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.16%20PM.jpeg" width="32%" alt="Rover Photo 1">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.17%20PM.jpeg" width="32%" alt="Rover Photo 2">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.18%20PM.jpeg" width="32%" alt="Rover Photo 3">
</p>
<p align="center">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.17%20PM%20(1).jpeg" width="48%" alt="Rover Photo 4">
  <img src="iotCar%20Photos/WhatsApp%20Image%202026-04-19%20at%204.31.17%20PM%20(2).jpeg" width="48%" alt="Rover Photo 5">
</p>

---

## ✨ Features & Capabilities

- 🌐 **Built-in Web Server UI**: A sleek, CSS-styled responsive dashboard hosted directly on the ESP32—no internet connection required!
- 🕹️ **Mode 1 - Manual Control**: A tactile D-Pad interface for full directional control (Forward, Reverse, Left, Right) featuring a rapid "Spin Combo" function.
- 🛡️ **Mode 2 - Obstacle Avoidance**: Autonomous navigation where the rover halts instantly prior to collision (<25cm) and intelligently alternates its turning vector.
- 🤖 **Mode 3 - Smart Explorer**: An advanced AI-like mode that scales motor speed proportionally based on distance. Enjoy auto-evasion maneuvers, "slow-mo" tight cornering, and "turbo bounds" on straightaways.
- 📏 **Live Distance Tracking**: HC-SR04 sonar data is continuously streamed right to the web dashboard UI in zero latency.

---

## 🧰 Hardware & Schematic

### 🔌 Parts Required
* **1×** ESP32 Development Board
* **1×** L298N Motor Driver
* **1×** HC-SR04 Ultrasonic Distance Sensor
* **1×** DC Motors & Robot Chassis
* **1×** 12V Battery Power Source

### ⚡ Wiring Guide

Ensure complete power compliance:
* **Battery** ➡️ L298N 12V
* **L298N GND** ➡️ ESP32 GND *(CRITICAL: Common Ground is Required)*
* **Motors** ➡️ L298N OUT1, OUT2, OUT3, OUT4
* **HC-SR04 Power** ➡️ ESP32 VIN (5V) & GND

> [!WARNING]
> Do NOT power the ESP32 directly from 12V unless going through a compatible step-down converter. Using the L298N's 5V breakout is an acceptable alternative if the onboard regulator jumper is intact.

#### 📍 ESP32 Pin Mapping Overview

| Component | Pin Function | ESP32 GPIO | Description |
|:---:|:---:|:---:|:---|
| L298N | IN1 | **GPIO 26** | Motor A Direction 1 |
| L298N | IN2 | **GPIO 27** | Motor A Direction 2 |
| L298N | IN3 | **GPIO 14** | Motor B Direction 1 |
| L298N | IN4 | **GPIO 12** | Motor B Direction 2 |
| L298N | ENA | **GPIO 25** | Motor A PWM Speed |
| L298N | ENB | **GPIO 33** | Motor B PWM Speed |
| HC-SR04 | TRIG | **GPIO 13** | Ultrasonic Pulse Transmitter |
| HC-SR04 | ECHO | **GPIO 34** | Ultrasonic Pulse Receiver *(Input-only)* |

---

## 🛠️ Arduino IDE Configuration

Follow these steps carefully to ensure the ESP32 compiles the web server and PWM hardware signals perfectly.

### 1. 📦 Install ESP32 Core Support
1. Open Arduino IDE and traverse to **File > Preferences**.
2. Add this URL to *Additional Boards Manager URLs*:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. Traverse to **Tools > Board > Boards Manager...**
4. Search for `esp32`.

> [!IMPORTANT]
> **CRITICAL VERSION SELECTION:** Select version **`2.0.17`** from the dropdown menu and click Install. This specific version is highly recommended for the stability of the Wi-Fi and PWM libraries executed in this sketch. Avoid newer 3.x.x series as they drastically refactored PWM headers!

### 2. ⚙️ Initialise Your Board Profile
* Go to **Tools > Board** and select your exact ESP32 model (e.g., *DOIT ESP32 DEVKIT V1* or *ESP32 Dev Module*).
* Verify that the upload speed is fixed to `115200`.

### 3. 🚀 Flash & Execute
1. Open the source code [`CODE/esp32_rover.ino`](CODE/esp32_rover.ino) in your IDE.
2. Hardcode your network credentials to map the Rover's Host:
   ```cpp
   const char* ssid     = "YOUR_SSID";
   const char* password = "YOUR_PASSWORD";
   ```
3. Physically interface the ESP32 to your PC via USB.
4. Mount the target port via **Tools > Port**.
5. Dispatch via **Upload**.

---

## 🏎️ Piloting the Rover

1. With the code flashed, initiate the **Serial Monitor** at baud rate `115200`.
2. Tap the **EN/RST** button on the physical ESP32 chassis.
3. Observe the prompt until `IP: xxx.xxx.xxx.xxx` bridges in the serial monitor.
4. Join your mobile/PC to the identical Wi-Fi access point.
5. Key the provided IP address into any modern web browser.
6. The digital cockpit awaits... Start exploring!

<br>

<div align="center">
  <i>Conceived and assembled for Robotics Development.</i>
  <br><br>
  <b>Reach out & connect with the developer:</b>
  <br><br>
  <a href="https://www.linkedin.com/in/yash-developer/"><img src="https://img.shields.io/badge/LinkedIn-%230077B5.svg?style=for-the-badge&logo=linkedin&logoColor=white" alt="LinkedIn Yash Developer"></a>
  <a href="https://www.instagram.com/yash.developer"><img src="https://img.shields.io/badge/Instagram-%23E4405F.svg?style=for-the-badge&logo=Instagram&logoColor=white" alt="Instagram @yash.developer"></a>
</div>
