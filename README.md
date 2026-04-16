# 🌾 Smart Rice Dryer with PID Control

An automated rice drying system built with Arduino that utilizes **PID (Proportional-Integral-Derivative)** control to maintain optimal drying temperatures. The system features an automatic Day/Night mode switching mechanism based on ambient light and safety monitoring for grain moisture levels.

## 🌟 Features

- **PID Temperature Control:** Uses a Dimmer module to precisely control heater power and maintain a stable temperature at **50°C**.
- **Automatic Day/Night Mode:**
  - **Night Mode (Dark):** Activates PID-controlled heating via the Dimmer module.
  - **Day Mode (Bright):** Activates a standard Relay (Direct Power).
- **Safety Moisture Check:** Monitors soil/grain moisture levels; automatically halts the system if moisture is too high (preventing mold/re-wetting).
- **Real-time Monitoring:**
  - **LCD Display:** Shows current temperature and PWM output.
  - **Serial Monitor:** Logs sensor data (Temp, Light, Moisture) every second.
- **RTC Integration:** DS3231 Real-Time Clock module included for future time-stamping capabilities.

## ⚙️ Hardware Components

| Component | Type/Spec |
| :--- | :--- |
| **Microcontroller** | Arduino Uno/Mega |
| **Temperature Sensor** | DS18B20 (Waterproof) |
| **Light Sensor** | LDR (Light Dependent Resistor) |
| **Moisture Sensor** | Soil Moisture Capacitive Sensor |
| **Actuators** | Dimmable Light Module (AC Heater Control), Relay Module |
| **Display** | LCD 16x2 I2C |
| **RTC** | DS3231 |
| **Others** | Resistors, Jumper Wires |

## 🔌 Wiring & Pin Configuration

| Component | Pin / Connection |
| :--- | :--- |
| **DS18B20 (Temp)** | Digital Pin 2 |
| **Relay (Day Mode)** | Digital Pin 5 |
| **LDR (Light)** | Analog Pin A0 |
| **Soil Moisture** | Analog Pin A1 |
| **Dimmer (Thyristor)** | Digital Pin 9 |
| **Dimmer (Sync)** | Digital Pin 3 |
| **I2C (LCD & RTC)** | SDA: A4, SCL: A5 (Arduino Uno) |

## 📊 System Logic

### 1. Mode Switching (Based on LDR)
The system detects ambient light to decide the heating method:
*   **Night Mode (LDR > 300):** It is dark. The system engages the **PID Controller**. The Dimmer adjusts the heater intensity to keep the temperature at 50°C.
*   **Day Mode (LDR < 200):** It is bright (sunlight available). The system turns **Relay ON** to use direct power (hybrid drying with sunlight).
*   **Hysteresis Zone (200 - 300):** Intermediate zone to prevent rapid switching.

### 2. PID Control
*   **Setpoint:** 50.0 °C
*   **Algorithm:** Uses the Ziegler-Nichols tuning method for precise calculation.
*   **Output:** Sends a PWM signal (0-255) to the Dimmable Light module to vary AC voltage.

### 3. Safety Moisture Check
*   Reads the soil moisture sensor.
*   **Emergency Stop:** If `soilMoisture >= 950`, the system enters an infinite loop and stops operation to prevent issues with excessive moisture.

## 🧠 PID Tuning

The PID parameters are calculated using the Ziegler-Nichols method with Ultimate Gain ($K_u$) and Ultimate Period ($T_u$):

```cpp
double Ku = 2.5;
double Tu = 2.0;

double Kp = 0.6 * Ku;           // Proportional
double Ki = 1.2 * Ku / Tu;      // Integral
double Kd = 0.075 * Ku * Tu;    // Derivative
```

## 📂 Installation

1.  **Install Libraries:** Ensure you have the following libraries installed via Arduino IDE Library Manager:
    *   `Wire`
    *   `OneWire`
    *   `DallasTemperature`
    *   `LiquidCrystal_I2C`
    *   `RTClib`
    *   `PID_v1`
    *   `Dimmable Light` (For AC phase control)

2.  **Upload:** Connect your Arduino and upload the `.ino` file.

3.  **Serial Monitor:** Open Serial Monitor at 9600 baud rate to see real-time data logs.

## ⚠️ Important Notes

*   **High Voltage:** The Dimmer module connects to AC mains voltage. **Extreme caution** is required when wiring the heater and dimmer.
*   **Sensor Calibration:** The LDR and Soil Moisture thresholds (300, 950) may need adjustment depending on your specific sensor model and environmental conditions.
*   **Relay Logic:** In this code, "Day Mode" triggers Relay 5 and disables the Dimmer/PID logic. Ensure your wiring matches this logic (e.g., Solar fan vs. Electrical heater).

## 📄 License

This project is open source for educational purposes.
