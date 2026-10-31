## Hardware Architecture & Wiring Topology

> **Firmware/source note:** This document describes the assembled dual-display hardware and the intended wiring. In the main firmware snapshot currently included in this repository, `DUM-E_Final.ino` initializes only one SH1106 display at I²C address `0x3C`; it does not initialize a second display at `0x3D`. The `ui-checks/Dual_Display_Test_UI` sketch is a standalone test and is not part of the product firmware. Keep this distinction in mind when reproducing the current repository build.


Dum-E relies on a highly efficient I2C bus topology to drive two independent displays from a single microcontroller without consuming excess GPIO pins. 

### 1. The Dual-Display I2C Bus (Address Multiplexing)
Standard SSD1306/SH1106 OLED displays ship with a hardcoded I2C address of `0x3C`. To allow the ESP32 to address two displays independently on the exact same two wires, a hardware modification was required.

* **Display 1 (D1 - Left / Master Clock):** Left at the factory default address `0x3C`.
* **Display 2 (D2 - Right / HUD & Spotify):** The SMD resistor on the back of the OLED PCB was physically de-soldered and moved from the `0x78` pad to the `0x7A` pad. This shifts its I2C address to `0x3D`. 

By establishing unique hardware addresses, both displays can share a single I2C bus. The ESP32's Core 0 graphics task simply targets `0x3C` for the clock and `0x3D` for the secondary UI.

### 2. Wiring Schematic (Current Tethered Setup)
The displays are wired in parallel to the ESP32's default I2C pins. 

| ESP32 Pin | Display 1 (0x3C) | Display 2 (0x3D) | Function / Purpose |
| :--- | :--- | :--- | :--- |
| **3V3** | VCC | VCC | 3.3V Logic Power |
| **GND** | GND | GND | Common Ground |
| **GPIO 21** | SDA | SDA | I2C Serial Data |
| **GPIO 22** | SCL | SCL | I2C Serial Clock |

> **Power Delivery (Tethered):** Currently, the system is powered directly via the ESP32’s onboard USB port. Connected to a workstation or a standard 5V wall adapter, the entire system (ESP32 + dual OLEDs) draws roughly ~150mA to 250mA, comfortably handled by the ESP32's onboard LDO voltage regulator.

---

## Roadmap: Wireless Autonomy Upgrade
To transition Dum-E from a tethered desk clock to a fully autonomous, battery-powered desk node, the hardware architecture will be upgraded using a **LiPo Battery** and a **TP4056 Charge Controller**.

### The Proposed Power Circuit:
1. **Power Source:** A single-cell `3.7V Lithium Polymer (LiPo)` or `18650 Li-ion` battery (approx. 2000mAh for days of continuous runtime).
2. **Charge Management (TP4056):** 
   * The battery connects to the `B+` and `B-` terminals of a TP4056 module.
   * The TP4056 handles safe USB-C charging, overcharge protection (cuts off at 4.2V), and deep-discharge protection (cuts off at 2.4V) to preserve battery health.
3. **Voltage Regulation (Boost to 5V):** 
   * A standard 3.7V LiPo voltage fluctuates as it drains, which can cause brownouts if fed directly into the ESP32's 5V pin.
   * A micro **DC-DC Boost Converter (Step-Up)** will be placed between the TP4056's `OUT+`/`OUT-` and the ESP32's `VIN` (5V) pin, guaranteeing a rock-solid 5.0V power rail regardless of the battery's charge level.

### Wireless Wiring Diagram (Conceptual):
```text
[ USB-C Power ] ---> [ TP4056 Module ]
                            │
                            ├─ (B+/B-) ─> [ 3.7V LiPo Battery ]
                            │
                            └─ (OUT+/-) ─> [ DC-DC 5V Boost ] ─> [ ESP32 5V (VIN) / GND ]