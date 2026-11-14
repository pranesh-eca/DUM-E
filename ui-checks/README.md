# UI Checks

These folders contain standalone Arduino sketches used to test individual UI/display ideas. They are **not dependencies** of the main Dum-E firmware and are not called by the product sketch.

## How to run a UI check

1. Open the specific sketch's `.ino` file in Arduino IDE.
2. Read that sketch's own pin, display, and library settings.
3. Select the appropriate ESP32 board and COM port.
4. Upload that sketch only when you want to test that UI.
5. To return to the product firmware, open and upload [`../firmware/DUM-E_Final/DUM-E_Final.ino`](../firmware/DUM-E_Final/DUM-E_Final.ino).

Each check may target a different display driver, display count, or wiring setup. Do not assume it has the same dependencies or behavior as the main firmware.
