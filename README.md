# AQI Monitor

A real-time **Air Quality Index (AQI) Monitor** using Arduino, gas sensors, and a Python script to visualize data dynamically using **Matplotlib**. The system reads gas concentrations and displays both real-time values on an LCD and live plots on a PC.

---

## 🛠️ Features

- Detects air quality using **MQ135 gas sensor**
- Displays:
  - CO₂, NH₃, and Toluene concentrations (in PPM)
  - Calculated AQI values for each gas
- Real-time display on **I2C 16x2 LCD**
- **Live graph plotting** using Python and Matplotlib
- Microcontroller: **Arduino UNO**

---

## 🧪 Components Used

- Arduino UNO
- MQ135 Gas Sensor
- I2C LCD (16x2)
- Jumper wires
- Breadboard
- PC (for Python visualization)

---

## 📈 Python Graph Visualization

A Python script using `pyserial` and `matplotlib` reads serial data from the Arduino and plots the gas concentrations and AQI levels **in real-time**.

### 📦 Requirements

Install required Python libraries:

```bash
pip install pyserial matplotlib
