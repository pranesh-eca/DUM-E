# Software Architecture

This page describes the main firmware currently present in this repository, not the broader planned product roadmap.

## Runtime split

```text
ESP32 (Arduino-ESP32)
│
├── Core 0: uiTaskCode()
│   ├── Samples joystick/button inputs
│   ├── Filters analog input noise and applies UI state transitions
│   ├── Scrolls track and artist text
│   └── Renders one 128 × 64 SH1106 OLED at I²C 0x3C
│
└── Arduino loop task (normally pinned to Core 1 by Arduino-ESP32)
    ├── Refreshes Spotify access token when needed
    ├── Polls current playback state approximately every 5 seconds
    ├── Reads current track and artist information
    └── Dispatches play/pause, previous/next, and volume requests
```

The UI task is created using `xTaskCreatePinnedToCore(..., 0)`. Network requests are made from `setup()`/`loop()` rather than the UI task, which helps keep blocking network operations away from the UI rendering loop.

## Important implementation details

- The display is initialized with `Adafruit_SH1106G` at `0x3C`.
- GPIO 32 and 33 are read as analog joystick axes; GPIO 25 is configured as a pull-up button input.
- The UI task has a 10,000-byte requested stack and runs at priority 1.
- Time uses `pool.ntp.org` and a fixed UTC+05:30 offset.
- Spotify access tokens are refreshed from a refresh token, and playback state is polled over HTTPS.
- `String` values and shared state are touched across execution contexts. Several shared variables are declared `volatile`, but `volatile` alone does not provide synchronization for compound operations or non-atomic data. Review shared-state safety before adding more cross-core behavior.
- HTTPS clients currently call `setInsecure()`, so certificate verification is disabled. See [`CONFIGURATION.md`](CONFIGURATION.md).
- Wi-Fi setup waits indefinitely for a connection; a future resilience pass should add timeout/retry/recovery behavior.

## Not in this source snapshot

The main firmware currently does not initialize the second display at `0x3D`, serve a local web dashboard, implement the Focus Sprint engine, or use SinricPro. The `ui-checks/` directory contains independent sketches and must not be drawn as integrated software modules in the architecture diagram.

The generated `assets/generated/boot_frames.h` is not included by the main sketch and therefore is not part of the compiled firmware as supplied.
