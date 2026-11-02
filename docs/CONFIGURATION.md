# Configuration and Credentials

The main firmware currently stores its configuration as constants near the top of `firmware/DUM-E_Final/DUM-E_Final.ino`:

```cpp
const char* ssid = "";
const char* password = "";
const char* clientId = "";
const char* clientSecret = "";
const char* refreshToken = "";
```

These values are deliberately blank in the checked-in source.

## Wi-Fi

Set `ssid` and `password` in your **local working copy** to the network you want the ESP32 to join. Do not post these values in README text, screenshots, issue reports, or commits.

## Spotify API configuration

The current sketch uses `SpotifyEsp32` plus direct HTTPS requests. Set the Spotify application client ID, client secret, and refresh token in your local build configuration. Only request and store the permissions that the app needs. Keep client secrets and refresh tokens private; anyone who obtains a valid token may be able to act with its granted permissions until it expires or is revoked.

Spotify's developer platform and API rules can change. Consult the [Spotify developer documentation](https://developer.spotify.com/documentation/web-api) for current application setup, redirect URI, token and endpoint requirements.

## Before committing

The current implementation reads these values directly from the `.ino` file; it does **not** yet load them from a separate local secrets header. Therefore:

1. Keep the repository's tracked constants blank.
2. Put real values only in a local, uncommitted working copy while building/testing.
3. Before staging or committing, restore the five constants to blank placeholders.
4. Inspect the diff and staged content for accidental credentials. A `.gitignore` rule does not hide secrets typed into a tracked `.ino` file.
5. If a real credential was ever committed or pushed, treat it as exposed: revoke/rotate it and remove it from Git history as appropriate. A later deletion alone does not make a leaked token safe.

A future non-behavioral cleanup should move configuration into a local-only header (for example, `secrets.h`, excluded by `.gitignore`) while committing a clearly named template. That change has not been made in this documentation step, so do not assume a separate header works with the current sketch.

## TLS note

The current code calls `WiFiClientSecure::setInsecure()` before Spotify HTTPS requests. That disables certificate validation and makes the connection vulnerable to interception by an attacker able to position themselves on the network path. Prefer proper CA/certificate validation before relying on the device on an untrusted network. This documentation step does not modify the firmware's TLS behavior.
