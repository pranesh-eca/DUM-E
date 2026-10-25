# DUM-E — Dual-Core RTOS Desk Node

A compact ESP32 desk display project built around dual 128×64 OLED screens, custom UI experiments, and Spotify playback telemetry.

> **Project status:** The main firmware has been uploaded to the ESP32 and tested by the author. This repository is being published in stages. The first commit contains project documentation and images; the main firmware and standalone UI checks will be added in subsequent commits.

## Product Gallery

<p align="center">
  <img src="assets/images/dume-home-ui.jpg" alt="DUM-E enclosure with dual OLED clock display" width="720">
</p>

<p align="center">
  <img src="assets/images/dume-bootup.jpg" alt="DUM-E boot screen" width="24%">
  <img src="assets/images/dume-spotify-ui.jpg" alt="DUM-E Spotify interface" width="24%">
  <img src="assets/images/dume-timer-ui.jpg" alt="DUM-E timer interface" width="24%">
</p>

## Project Overview

DUM-E is a small desk node built with an ESP32 and two 0.96-inch monochrome OLED displays. The enclosure is custom 3D-printed, and the interface is designed to provide glanceable information without requiring a dedicated physical control panel.

The project explores:
- Dual OLED displays on a shared I²C bus, using distinct display addresses.
- ESP32 dual-core task separation with FreeRTOS.
- Custom monochrome display interfaces and boot animation assets.
- Spotify playback telemetry.
- A Python utility that converts video frames into C/C++ bitmap data for embedded graphics.

## Hardware

- ESP32 development board
- Two 128×64 OLED modules (SSD1306/SH1106-compatible; confirm the controller on your modules)
- Custom 3D-printed enclosure
- USB power for the current build

Both OLEDs share the I²C bus. Display 1 uses address `0x3C`; Display 2 is configured at `0x3D` by changing the module's address-selection resistor/jumper configuration.

See [Hardware notes](docs/HARDWARE.md) and the [dual-OLED connection reference](assets/images/dual-oled-i2c-wiring.png).

## Repository Layout

| Path | Purpose |
|---|---|
| `firmware/` | Main firmware; published in a later project update |
| `ui-checks/` | Standalone display/UI test sketches; not dependencies of the main firmware |
| `tools/` | Helper scripts for preparing embedded graphics |
| `assets/images/` | Product photos and hardware references |
| `assets/generated/` | Generated graphics assets |
| `docs/` | Hardware, configuration, architecture and build notes |

## Firmware Scope

The main firmware is the only sketch intended to be flashed for normal device operation. The sketches in `ui-checks/` are separate tests that can be uploaded individually to validate a display or interface; they are not called by the main firmware.

The detailed source will be included in a later staged commit. Feature descriptions in this repository will be kept aligned with the firmware actually published.

## Documentation

- [Hardware](docs/HARDWARE.md)
- [Software architecture](docs/SOFTWARE_ARCHITECTURE.md)
- [Build and flash instructions](docs/BUILD_AND_FLASH.md)
- [Configuration and credentials](docs/CONFIGURATION.md)

## Tools

`tools/video_to_header.py` converts supported video frames into monochrome bitmap arrays for embedded graphics. The utility and generated asset will be published in a subsequent commit.

## Safety and configuration

Do not commit Wi-Fi passwords, Spotify client secrets, refresh tokens, private keys, or local configuration files. Review the configuration guide before building or publishing firmware.

## License

This project is distributed under the terms in [LICENSE](LICENSE). Third-party libraries and assets remain subject to their respective licenses.
