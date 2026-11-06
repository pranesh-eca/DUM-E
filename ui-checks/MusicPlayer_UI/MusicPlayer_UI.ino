#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SpotifyEsp32.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ==========================================
// 1. CONFIGURATION & CREDENTIALS
// ==========================================
const char* ssid         = "YOUR_WIFI_SSID";
const char* password     = "YOUR_WIFI_PASSWORD";
const char* clientId     = "YOUR_SPOTIFY_CLIENT_ID";
const char* clientSecret = "YOUR_SPOTIFY_CLIENT_SECRET";
const char* refreshToken = "YOUR_SPOTIFY_REFRESH_TOKEN";

// Display Configuration (Disp2 / Right Display)
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define DISP2_ADDRESS 0x3D // Hardware address for Dum-E Display 2

Adafruit_SSD1306 disp2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Spotify spotify(clientId, clientSecret, refreshToken);

// ==========================================
// 2. STATE & PLAYBACK VARIABLES
// ==========================================
volatile bool isPlaying = false;
volatile int  volume = 85;         
volatile int  songProgress = 0;    
volatile float preciseProgress = 0.0;

// Track Information
String trackName = "Loading Track..."; 
String artistName = "Loading Artist...";
int trackNameWidth = 200;               
int artistNameWidth = 200;

String customAccessToken = "";
unsigned long tokenStartTime = 0;

// ==========================================
// 3. SPOTIFY API AUTH HELPER
// ==========================================
void refreshAccessToken() {
  if (customAccessToken != "" && (millis() - tokenStartTime < 3000000)) return; 
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, "https://accounts.spotify.com/api/token");
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  String payload = "grant_type=refresh_token&refresh_token=" + String(refreshToken) + 
                   "&client_id=" + String(clientId) + "&client_secret=" + String(clientSecret);
                   
  if (http.POST(payload) == 200) {
    String response = http.getString();
    int startIndex = response.indexOf("\"access_token\":\"") + 16;
    int endIndex = response.indexOf("\"", startIndex);
    if (startIndex > 15 && endIndex > startIndex) {
      customAccessToken = response.substring(startIndex, endIndex);
      tokenStartTime = millis();
    }
  }
  http.end();
}

// ==========================================
// 4. CORE 0: DEDICATED MUSIC PLAYER UI TASK
// ==========================================
TaskHandle_t uiTask;

void uiTaskCode(void * parameter) {
  int scrollX = 5;                  
  int artistScrollX = 5; 
  unsigned long lastScrollTime = 0;
  unsigned long lastProgressTick = 0;

  for(;;) {
    unsigned long currentMillis = millis();
    bool uiNeedsUpdate = false;

    // Scrolling Marquee Timing
    if (currentMillis - lastScrollTime > 30) {
      if (trackNameWidth > 90) { 
        scrollX--;
        if (scrollX < -trackNameWidth) scrollX = 128; 
      } else scrollX = 5;

      if (artistNameWidth > 115) { 
        artistScrollX--;
        if (artistScrollX < -artistNameWidth) artistScrollX = 128; 
      } else artistScrollX = 5;
      
      uiNeedsUpdate = true; 
      lastScrollTime = currentMillis;
    }

    // Progress Bar Interpolation (Visual Estimate)
    if (isPlaying) {
      if (currentMillis - lastProgressTick > 1000) { 
        preciseProgress += 0.4;
        if (preciseProgress > 100.0) preciseProgress = 100.0;
        songProgress = (int)preciseProgress;
        uiNeedsUpdate = true;
        lastProgressTick = currentMillis;
      }
    } else {
      lastProgressTick = currentMillis;
    }

    // ==========================================
    // RENDER: PURE SPOTIFY PLAYER UI
    // ==========================================
    if (uiNeedsUpdate) {
      disp2.clearDisplay();
      disp2.setTextColor(SSD1306_WHITE);

      // Track Title
      disp2.setTextSize(2);
      disp2.setCursor(scrollX, 5); 
      disp2.print(trackName);

      // Volume Badge (Masks background for clean scroll)
      disp2.fillRect(95, 0, 33, 22, SSD1306_BLACK);
      disp2.setTextSize(1);
      disp2.setCursor(102, 5); 
      if (volume < 100) disp2.print(" ");
      if (volume < 10)  disp2.print(" ");
      disp2.print(volume);
      disp2.print(F("%"));

      // Artist Subtitle
      disp2.setTextSize(1);
      disp2.setCursor(artistScrollX, 25);
      disp2.print(artistName); 
      disp2.fillRect(115, 23, 13, 12, SSD1306_BLACK);
      
      // Progress Bar
      disp2.drawRoundRect(5, 40, 118, 6, 3, SSD1306_WHITE);
      int barWidth = map(songProgress, 0, 100, 0, 118);
      if (barWidth > 0) disp2.fillRoundRect(5, 40, barWidth, 6, 3, SSD1306_WHITE);

      // Previous Button (Double Triangles)
      disp2.fillTriangle(24, 55, 32, 50, 32, 60, SSD1306_WHITE);
      disp2.fillTriangle(32, 55, 40, 50, 40, 60, SSD1306_WHITE);

      // Play / Pause Button
      if (isPlaying) {
        disp2.fillRect(59, 50, 3, 10, SSD1306_WHITE);
        disp2.fillRect(66, 50, 3, 10, SSD1306_WHITE);
      } else {
        disp2.fillTriangle(69, 55, 59, 50, 59, 60, SSD1306_WHITE);
      }

      // Next Button (Double Triangles)
      disp2.fillTriangle(96, 55, 88, 50, 88, 60, SSD1306_WHITE);
      disp2.fillTriangle(104, 55, 96, 50, 96, 60, SSD1306_WHITE);

      disp2.display();
    }
    
    vTaskDelay(20 / portTICK_PERIOD_MS); 
  }
}

// ==========================================
// 5. SETUP & CORE 1 NETWORK LOOP
// ==========================================
void setup() {
  Serial.begin(115200);
  delay(250);

  // Initialize specifically on Disp2 (0x3D)
  if(!disp2.begin(SSD1306_SWITCHCAPVCC, DISP2_ADDRESS)) {
    Serial.println(F("Disp2 (0x3D) initialization failed!"));
    for(;;); 
  }
  
  disp2.setRotation(0); 
  disp2.setTextWrap(false);
  disp2.clearDisplay();
  disp2.setTextSize(1);
  disp2.setTextColor(SSD1306_WHITE);
  disp2.setCursor(15, 28);
  disp2.print("Connecting WiFi...");
  disp2.display();
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(300);

  spotify.begin();

  // Pin Graphics rendering strictly to Core 0
  xTaskCreatePinnedToCore(uiTaskCode, "UI_Task", 10000, NULL, 1, &uiTask, 0);
}

void loop() {
  unsigned long currentMillis = millis();

  // Background Telemetry Sync (Every 5 seconds)
  static unsigned long lastTrackCheck = 0;
  if (currentMillis - lastTrackCheck > 5000) {
    refreshAccessToken();
    if (customAccessToken != "") {
      WiFiClientSecure client;
      client.setInsecure();
      HTTPClient http;
      http.begin(client, "https://api.spotify.com/v1/me/player");
      http.addHeader("Authorization", "Bearer " + customAccessToken);
      int httpCode = http.GET();
      
      if (httpCode == 200) {
        String payload = http.getString();
        
        // Parse Play State
        bool apiIsPlaying = (payload.indexOf("\"is_playing\" : true") > 0 || 
                             payload.indexOf("\"is_playing\":true") > 0 || 
                             payload.indexOf("\"is_playing\": true") > 0);
        isPlaying = apiIsPlaying;
        
        // Parse Volume
        int volSearch = payload.indexOf("\"volume_percent\"");
        if (volSearch != -1) {
          int colon = payload.indexOf(":", volSearch);
          if (colon != -1) {
            int comma   = payload.indexOf(",", colon);
            int bracket = payload.indexOf("}", colon);
            int endVol  = (comma == -1) ? bracket : ((bracket != -1 && bracket < comma) ? bracket : comma);
            
            if (endVol != -1) {
              String volStr = payload.substring(colon + 1, endVol);
              volStr.trim();
              volume = volStr.toInt();
            }
          }
        }

        // Fetch Track and Artist using Spotify Library
        String newTrack = spotify.current_track_name();
        if (newTrack != "" && newTrack != "Something went wrong" && newTrack != "null") {
          if (newTrack != trackName) {
            songProgress = 0;
            preciseProgress = 0.0;
          }
          trackName = newTrack;
          artistName = spotify.current_artist_names();
          trackNameWidth = trackName.length() * 12;
          artistNameWidth = artistName.length() * 6;
        }
      } else if (httpCode == 204) {
        isPlaying = false;
      }
      http.end();
    }
    lastTrackCheck = currentMillis;
  }
  
  delay(20);
}