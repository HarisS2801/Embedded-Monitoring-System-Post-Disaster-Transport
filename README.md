# Embedded Monitoring System for Post-Disaster Supply Transport

> An ATmega328P-based embedded monitoring system designed to continuously monitor temperature and humidity during the transportation and storage of temperature-sensitive post-disaster relief supplies.

![ATmega328P](https://img.shields.io/badge/MCU-ATmega328P-blue)
![Embedded C](https://img.shields.io/badge/Language-Embedded%20C-orange)
![UART](https://img.shields.io/badge/Communication-UART-green)
![Embedded Systems](https://img.shields.io/badge/Domain-Embedded%20Systems-purple)
![Application](https://img.shields.io/badge/Application-Disaster%20Relief-red)
![University](https://img.shields.io/badge/University-University%20of%20Jaffna-success)
![Status](https://img.shields.io/badge/Status-Completed-success)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## 📌 Project Overview

Post-disaster relief operations require the safe transportation and storage of essential supplies such as medicines, vaccines, food items, and medical kits.

Many of these supplies are sensitive to environmental conditions. Exposure to temperatures or humidity levels outside their safe operating ranges can affect their quality, effectiveness, and usability.

This project presents a portable and low-power **embedded environmental monitoring system** designed to continuously monitor temperature and humidity during the transportation and storage of relief supplies.

The system is built around a standalone **ATmega328P microcontroller** and provides real-time environmental monitoring with threshold-based alerts. When the measured temperature or humidity exceeds predefined safety limits, the system generates an alert so that timely corrective action can be taken.

The design focuses on being **portable, low-cost, reliable, and suitable for resource-constrained environments**, making it applicable to disaster relief and humanitarian logistics.

---

## 🎯 Objectives

The main objectives of this project are:

- Develop a standalone ATmega328P-based monitoring system.
- Continuously monitor temperature and humidity.
- Detect environmental conditions outside predefined safety limits.
- Generate real-time alerts for abnormal conditions.
- Display environmental measurements through a monitoring interface.
- Implement UART-based communication.
- Develop a portable and low-power system architecture.
- Provide a cost-effective monitoring solution for disaster relief operations.
- Improve the safety and reliability of temperature-sensitive relief supplies during transportation and storage.

---

## 🏗️ System Architecture

```text
                    ┌──────────────────────┐
                    │   Temperature Sensor │
                    └──────────┬───────────┘
                               │
                               │
                    ┌──────────▼───────────┐
                    │    Humidity Sensor   │
                    └──────────┬───────────┘
                               │
                               ▼
                    ┌──────────────────────┐
                    │      ATmega328P      │
                    │   Microcontroller    │
                    └──────────┬───────────┘
                               │
                ┌──────────────┼──────────────┐
                │              │              │
                ▼              ▼              ▼
        Threshold Analysis   UART       Alert System
                │              │              │
                │              ▼              │
                │      ┌───────────────┐      │
                │      │   Monitoring  │      │
                │      │   Interface   │      │
                │      └───────────────┘      │
                │                             │
                └──────────────┬──────────────┘
                               │
                               ▼
                       Environmental
                       Status / Alert
```
## ⚙️ Working Principle

The monitoring system continuously acquires environmental measurements from temperature and humidity sensors.

The **ATmega328P** processes the sensor data and compares the measured values against predefined threshold values.

The general operating sequence is:

```text
                         START
                           │
                           ▼
                 Initialize ATmega328P
                           │
                           ▼
                   Initialize Sensors
                           │
                           ▼
             Read Temperature & Humidity
                           │
                           ▼
                  Process Sensor Data
                           │
                           ▼
               Compare With Thresholds
                           │
                    ┌──────┴──────┐
                    │             │
                    ▼             ▼
                 NORMAL        ABNORMAL
                    │             │
                    ▼             ▼
              Display Data    Activate Alert
                    │             │
                    └──────┬──────┘
                           │
                           ▼
                    Continue Monitoring
```

---

## 🔍 Environmental Monitoring

### 🌡️ Temperature Monitoring

The temperature sensor continuously measures the environmental temperature surrounding the transported or stored supplies.

The measured temperature is processed by the ATmega328P and compared with the predefined safe temperature range.

If the measured temperature exceeds the specified threshold, the system activates the corresponding alert mechanism.

### 💧 Humidity Monitoring

Humidity is continuously monitored because excessive or unsuitable humidity levels can negatively affect certain relief supplies and storage environments.

The measured humidity value is compared against the configured threshold.

If the humidity exceeds the acceptable range, the system generates an appropriate warning.

---

## 🚨 Alert Mechanism

The system uses a threshold-based decision mechanism to identify abnormal environmental conditions.

```text
                Sensor Measurement
                        │
                        ▼
                Compare with Limit
                        │
               ┌────────┴────────┐
               │                 │
               ▼                 ▼
         Within Safe Range   Limit Exceeded
               │                 │
               ▼                 ▼
        Normal Operation       ALERT
               │                 │
               └────────┬────────┘
                        │
                        ▼
                 Continue Monitoring
```

This mechanism allows abnormal environmental conditions to be detected quickly and provides an opportunity for timely intervention.

---

## 📡 UART Communication

UART communication is used to transfer sensor readings between the embedded system and the monitoring interface.

The communication architecture can be represented as:

```text
Temperature Sensor ──┐
                     │
Humidity Sensor ─────┤
                     ▼
               ┌────────────┐
               │ ATmega328P │
               │     MCU    │
               └──────┬─────┘
                      │
                     UART
                      │
                      ▼
             Monitoring Interface
                      │
                      ▼
              Real-Time Readings
```

This allows sensor information to be transmitted and displayed in real time.

---

## 🧩 Hardware Components

The major components used in the system include:

- **ATmega328P Microcontroller**
- **Temperature Sensor**
- **Humidity Sensor**
- **Display / Monitoring Interface**
- **Resistors**
- **Capacitors**
- **Power Supply**
- **Supporting Electronic Components**

---

## 💻 Software & Technologies

| Category | Technology |
|----------|------------|
| Microcontroller | ATmega328P |
| Programming Language | Embedded C |
| Communication | UART |
| Environmental Monitoring | Temperature & Humidity Sensors |
| Interface | Real-Time Monitoring Interface |
| System Type | Embedded Monitoring System |
| Application | Disaster Relief Logistics |

---

## 🔌 Hardware Design

The system was designed around a **standalone ATmega328P microcontroller** rather than relying on a complete development board.

This approach provides greater control over the hardware architecture and allows the system to be optimized for:

- Portability
- Low power consumption
- Cost reduction
- Compact implementation
- Resource-constrained environments

### Circuit Diagram

Add your circuit diagram here:

```markdown
![Circuit Diagram](Images/Circuit_Diagram.png)
```

> **Note:** Make sure the filename matches the actual circuit diagram uploaded to your `Images` folder.

---

## 🖥️ Monitoring Interface

A user-friendly monitoring interface was developed to display environmental measurements received from the embedded system.

The interface provides real-time visibility of:

- Temperature
- Humidity
- Environmental status
- Alert conditions

### Interface Screenshot

```markdown
![Monitoring Interface](Images/Monitoring_Interface.png)
```

> Replace `Images/Monitoring_Interface.png` with the actual filename of your interface screenshot.

---
## 📊 Results

The developed embedded monitoring system was tested under different environmental conditions.

The system successfully demonstrated:

- Continuous temperature monitoring
- Continuous humidity monitoring
- Real-time display of sensor measurements
- Threshold-based environmental monitoring
- Detection of abnormal temperature conditions
- Detection of abnormal humidity conditions
- Real-time alert generation
- UART-based data communication
- Reliable operation using the ATmega328P microcontroller

---

## 🧪 Testing

Testing was performed under different environmental conditions to verify the operation of the monitoring and alert mechanisms.

| Test Condition | Expected Response |
|----------------|------------------|
| Normal Temperature | Normal operation |
| High Temperature | Temperature alert |
| Normal Humidity | Normal operation |
| High Humidity | Humidity alert |
| Temperature & Humidity within limits | Normal monitoring |
| Multiple abnormal conditions | Appropriate alerts |

---

## 📈 System Workflow

The complete monitoring workflow can be summarized as follows:

```text
             Environmental Conditions
                       │
                       ▼
             ┌────────────────────┐
             │ Temperature Sensor │
             └─────────┬──────────┘
                       │
                       ▼
             ┌────────────────────┐
             │   Humidity Sensor  │
             └─────────┬──────────┘
                       │
                       ▼
                ┌─────────────┐
                │  ATmega328P │
                │     MCU     │
                └──────┬──────┘
                       │
                       ▼
                Data Processing
                       │
                       ▼
              Threshold Comparison
                       │
                 ┌─────┴─────┐
                 │           │
                 ▼           ▼
              Normal      Abnormal
                 │           │
                 ▼           ▼
            Display Data    Alert
                 │           │
                 └─────┬─────┘
                       │
                       ▼
                 UART Interface
                       │
                       ▼
                User Monitoring
```

---
