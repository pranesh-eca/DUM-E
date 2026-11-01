# Build and Flash

This guide covers the **main product sketch only**: `firmware/DUM-E_Final/DUM-E_Final.ino`. Sketches in `ui-checks/` are independent programs and are not part of this build.

## 1. Required software

- Arduino IDE
- ESP32 board support package for Arduino
- USB data cable and the ESP32's COM port driver, if required by your board

Install these libraries through Arduino IDE Library Manager:

- **Adafruit GFX Library**
- **Adafruit SH110X**
- **SpotifyEsp32**
- **ArduinoJson** (a dependency of SpotifyEsp32 in the current upstream library)

The main sketch also uses ESP32-core libraries such as `WiFi`, `WiFiClientSecure`, `HTTPClient`, `Wire`, and `time`; these are supplied by the ESP32 Arduino core rather than installed as separate display libraries.

Upstream references:

- [Adafruit SH110X](https://github.com/adafruit/Adafruit_SH110X)
- [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library)
- [SpotifyEsp32](https://github.com/FinianLandes/SpotifyEsp32)

The archive does not record exact tested versions of the ESP32 board package or contributed libraries. For reproducible builds, record the versions you successfully compile and test with before tagging a release.

## 2. Configure the sketch

Read [`CONFIGURATION.md`](CONFIGURATION.md) before entering Wi-Fi or Spotify credentials. The five credential constants in the sketch are currently blank placeholders. Do not commit real credentials or refresh tokens.

## 3. Open the correct sketch

In Arduino IDE, open:

```text
firmware/DUM-E_Final/DUM-E_Final.ino
```

Open that `.ino` from its matching `DUM-E_Final` folder. Do not upload a sketch from `ui-checks/` when you intend to run the product firmware.

## 4. Board settings

Select the ESP32 board profile that matches your actual development board and select the matching COM port. If your existing known-good setup uses the **Huge APP (3 MB)** partition scheme, you may keep that setting; the supplied source archive does not include a verified board-profile export, and this single-display sketch does not itself include the generated boot animation header or the dashboard described in the broader project brief. Confirm the menu label against your installed ESP32 board package.

## 5. Compile and upload

1. Connect the ESP32 by USB.
2. Select the board and COM port.
3. Verify the local credentials are configured without exposing them to a repository.
4. Click **Verify** to compile.
5. Click **Upload**.
6. Open Serial Monitor at **115200 baud**.

The firmware obtains time from `pool.ntp.org` with a fixed UTC+05:30 offset. It also contacts Spotify endpoints, so valid network access and Spotify configuration are required for playback features.

## 6. Current hardware assumptions

- Main firmware display: SH1106-compatible 128 × 64 OLED at I²C `0x3C`.
- Joystick X axis: GPIO 32.
- Joystick Y axis: GPIO 33.
- Joystick/button input: GPIO 25, configured with `INPUT_PULLUP`.
- Power: USB 5 V through the ESP32 development board.

The hardware document describes a second OLED at `0x3D`, but the main sketch in this repository does not initialize it yet. Do not use the standalone dual-display test as evidence that the product sketch renders on both displays.

## 7. Troubleshooting

| Symptom | Check |
|---|---|
| Compile error for an include | Install the missing library and its dependencies; confirm the ESP32 board package is installed. |
| Upload fails | Check the COM port, USB data cable, board selection, and any required boot-button procedure for your board. |
| Firmware appears stuck at startup | The current `setup()` waits while Wi-Fi is disconnected. Check SSID/password and network availability. |
| Clock does not show valid time | Check Wi-Fi and outbound NTP access. |
| Spotify content is missing | Check credentials, refresh token, Spotify account/API access, and whether playback is active on a supported device. |
| Display is blank or distorted | Confirm SH1106-compatible display hardware, 3.3 V wiring, common ground, SDA/SCL wiring, and address `0x3C`. |

Before calling a build reproducible, compile it using the documented dependency versions and test it on the actual assembled hardware.
