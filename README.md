# ESP32 Automatic Fish Feeder 🐟⏱️

[![ESP32](https://img.shields.io/badge/Microcontroller-ESP32-blue.svg)](https://www.espressif.com/)
[![Framework](https://img.shields.io/badge/Framework-Arduino-teal.svg)](https://www.arduino.cc/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub Repository](https://img.shields.io/badge/GitHub-automatic--fish--feeder-181717?logo=github)](https://github.com/HarithaShree-S/automatic-fish-feeder)

An automated, reliable fish feeding system engineered around the **ESP32** microcontroller. The feeder synchronizes precise local time over Wi-Fi via Network Time Protocol (NTP) and controls a servo motor to dispense calibrated food portions on a scheduled daily routine.

---

## 🌟 Key Features

- **Accurate Timekeeping**: Connects to NTP time servers (`pool.ntp.org`) over Wi-Fi, eliminating the need for an external RTC module.
- **Multiple Daily Schedules**: Flexible, programmable feeding schedules (morning, afternoon, evening).
- **Servo-Driven Dispenser**: Calibrated food drum/hopper movement with configurable open/close angles and dispensing durations.
- **Offline & Reconnect Resilience**: Retains scheduled feeding operations even through transient Wi-Fi drops.
- **Modular Test Progression**: Organized firmware test suite for incremental validation (GPIO/Serial -> Servo -> NTP -> Main System).

---

## 📁 Repository Structure

```text
automatic-fish-feeder/
├── hardware/
│   └── CAD_Models/               # 3D printable CAD models (STL, STEP, project files)
├── simulation/                   # Wokwi / circuit simulation configurations
├── software/
│   ├── 01_Blink_Serial_Test/     # Initial ESP32 validation & Serial Monitor test (115200 baud)
│   ├── 02_Servo_Sweep_Test/      # Servo motor angle and dispenser rotation test (0°, 90°, 180°)
│   ├── 03_NTP_Time_Test/         # Wi-Fi connection and NTP time synchronization test
│   └── 04_Main_Feeder_System/    # Integrated fish feeder firmware with scheduled feeding
├── .gitignore                    # Build artifacts, IDE configs, and CAD backup filters
└── README.md                     # Project overview and documentation
```

---

## 🛠️ Hardware Requirements & BOM

| Component | Specification / Model | Qty | Description |
| :--- | :--- | :---: | :--- |
| **Microcontroller** | ESP32 DevKit V1 / NodeMCU | 1 | Main controller with built-in Wi-Fi |
| **Actuator** | Micro Servo (SG90 or MG90S) | 1 | Drives food dispensing gate / drum |
| **Power Supply** | 5V DC (min 2A recommended) | 1 | Powers ESP32 and handles servo peak current |
| **Decoupling Capacitor** | 100 µF – 470 µF Electrolytic | 1 | Placed across servo 5V and GND rail |
| **Status Indicator** | Built-in LED (GPIO 2) or external | 1 | Visual indicator during dispensing |
| **Feeder Assembly** | 3D Printed Hopper / Auger / Drum | 1 | Custom 3D CAD printed container |
| **Prototyping** | Jumper Wires & Breadboard / PCB | - | Interconnects and circuit wiring |

---

## 🔌 Circuit Pinout & Wiring

> [!IMPORTANT]
> **Always connect all grounds together (Common GND)**. Never power a servo directly from the ESP32's 3.3V pin. Use an external 5V supply for the servo with a shared ground between the power supply and the ESP32.

| Servo Wire | Connection Target | Description |
| :--- | :--- | :--- |
| **Signal (Orange/Yellow)** | **ESP32 GPIO 18** | PWM Control Signal |
| **VCC (Red)** | **External +5V Power** | Servo Motor Power |
| **GND (Brown/Black)** | **Common Ground (GND)** | Shared Ground between ESP32 and 5V PSU |

---

## 💻 Software Modules & Progression

The firmware suite is organized incrementally to ensure thorough testing before deploying the complete system:

1. **[`01_Blink_Serial_Test`](file:///e:/Automatic%20Fish%20Feeder/software/01_Blink_Serial_Test/01_Blink_Serial_Test.ino)**:
   - Validates development environment, board detection, onboard LED blinking, and 115200-baud UART serial output.
2. **[`02_Servo_Sweep_Test`](file:///e:/Automatic%20Fish%20Feeder/software/02_Servo_Sweep_Test/02_Servo_Sweep_Test.ino)**:
   - Utilizes `ESP32Servo` to sweep through 0°, 90°, and 180° to calibrate dispenser opening angles and mechanical limits.
3. **[`03_NTP_Time_Test`](file:///e:/Automatic%20Fish%20Feeder/software/03_NTP_Time_Test/03_NTP_Time_Test.ino)**:
   - Connects to local Wi-Fi, contacts an NTP pool (`pool.ntp.org`), and configures timezone offset (e.g. UTC+5:30).
4. **[`04_Main_Feeder_System`](file:///e:/Automatic%20Fish%20Feeder/software/04_Main_Feeder_System/04_Main_Feeder_System.ino)**:
   - Production firmware combining time-based daily schedules, automated food dispensing cycles, and status alerts.

---

## 🚀 Getting Started

### 1. Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) (version 2.0+) or [PlatformIO](https://platformio.org/)
- ESP32 Board package installed via Arduino Boards Manager (`esp32` by Espressif Systems)
- Required Arduino Libraries:
  - `ESP32Servo` (Install via Arduino Library Manager)
  - `WiFi` (Built-in with ESP32 board package)
  - `time.h` (Standard C/ESP-IDF library)

### 2. Configuration
Before uploading `03_NTP_Time_Test` or `04_Main_Feeder_System`, configure your credentials in the `.ino` file:

```cpp
// Wi-Fi Credentials
const char* ssid     = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Timezone Offset (in seconds)
// Example: UTC + 5:30 = 5.5 * 3600 = 19800
const long gmtOffset_sec = 19800;
const int daylightOffset_sec = 0;
```

### 3. Uploading Firmware
1. Connect your ESP32 board via USB.
2. Select your board model (e.g., `ESP32 Dev Module`) and correct COM Port.
3. Set the Serial Monitor baud rate to **115200**.
4. Compile and flash the sketch.

---

## 🔮 Future Roadmap

- [ ] Web dashboard / captive portal for setting Wi-Fi and schedules without re-flashing.
- [ ] Ultrasonic distance sensor (HC-SR04) or load cell to monitor food hopper level.
- [ ] MQTT / Home Assistant integration for IoT smart home monitoring.
- [ ] Push notification alerts on low food or feed completion.

---

## 📄 License

This project is open-source and licensed under the [MIT License](LICENSE).
