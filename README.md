# IoT-Solar-Tracker-Energy-Optimization
Embedded dual-LDR single-axis solar tracking and off-grid battery storage controller that dynamically aligns solar panels from East to West to maximize energy harvesting in remote areas.
# IoT Solar Panel Sun-Tracking & Energy Optimization System

An embedded solar energy harvesting and battery storage controller designed for **off-grid and remote areas** where grid power is unavailable. Using dual Light Dependent Resistors (LDRs) and closed-loop servo/actuator control, the system tracks the sun's movement from **East to West**, maximizes daytime solar power generation, stores energy in a battery bank, and automatically switches on night-time load power.

## System Architecture
+------------------------+     ADC (A0 / A1)     +-------------------------+
|  East & West LDR       | --------------------> |                         |
|  Irradiance Sensors    |                       |    Microcontroller      |
+------------------------+                       |  (Embedded C/C++ Logic) |
|                         |
+------------------------+     ADC (A2)          |  - Sun Differential FSM |
|  Solar Panel & Battery | --------------------> |  - Day/Night Load Logic |
|  Voltage Divider Sense |                       +------------+------------+
+------------------------+                                    |
+-----------------+-----------------+
|                                   |
PWM (Pin D9)                     Digital Out (Pin D7)
|                                   |
v                                   v
+--------------------------+        +--------------------------+
|  Solar Tracker Actuator  |        |  Battery Charge / Night  |
|  (Rotates East -> West)  |        |  Load Relay Control      |
+--------------------------+        +--------------------------+

# How It Works
1. **East-to-West Sun Tracking:** Compares analog irradiance readings from East-facing and West-facing LDR sensors. When the differential exceeds a deadband threshold, a PWM actuator rotates the solar panel toward the stronger light source to keep the panel perpendicular to sunlight.
2. **Night Reset & Battery Conservation:** When ambient light drops below the dusk threshold, the panel parks back to its East starting position for the next morning and puts tracking actuators to sleep to save battery power.
3. **Off-Grid Night Load Automation:** Monitors battery voltage (`ADC`) and automatically enables the DC/AC night load relay after sunset while protecting the battery from deep discharge.

## Hardware Pinout
| Component | Interface | Pin |
| :--- | :--- | :--- |
| East LDR Sensor (Voltage Divider) | Analog Input | `A0` |
| West LDR Sensor (Voltage Divider) | Analog Input | `A1` |
| Battery Voltage Sense Divider | Analog Input | `A2` |
| Solar Panel Tracking Servo / Actuator | PWM Output | `D9` |
| Night Load Relay (Active Low) | Digital Output | `D7` |
| Low-Battery Warning LED | Digital Output | `D13` |
