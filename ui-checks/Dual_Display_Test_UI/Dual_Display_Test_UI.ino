#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- Display Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Instantiate both screens independently
Adafruit_SSD1306 displayClock(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 displayCalendar(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(250);

  if(!displayClock.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("Display 1 (Clock) failed! Check wiring."));
    for(;;);
  }
  
  if(!displayCalendar.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
    Serial.println(F("Display 2 (Calendar) failed! Address issue."));
    for(;;);
  }

  displayClock.setRotation(0); 
  displayClock.setTextWrap(false);
  
  displayCalendar.setRotation(0); 
  displayCalendar.setTextWrap(false);
}

void loop() {
  // Dummy time variables for the UI test
  static int dummyHour = 11;
  static int dummyMin = 52;
  static int dummySec = 55;

  // Increment dummy time
  dummySec++;
  if (dummySec > 59) { dummySec = 0; dummyMin++; }
  if (dummyMin > 59) { dummyMin = 0; dummyHour++; }
  if (dummyHour > 12) { dummyHour = 1; }

  // --------------------------------------------------
  // RENDER SCREEN 1: FULL DISPLAY CLOCK (0x3C)
  // --------------------------------------------------
  displayClock.clearDisplay();
  displayClock.setTextColor(SSD1306_WHITE);

  char timeHour[3], timeMin[3], timeSec[3];
  sprintf(timeHour, "%02d", dummyHour);
  sprintf(timeMin, "%02d", dummyMin);
  sprintf(timeSec, "%02d", dummySec);

  // 1. Massive Size 5 Time (100% untouched, Y=12)
  displayClock.setTextSize(5);
  
  displayClock.setCursor(2, 12); 
  displayClock.print(timeHour);
  
  displayClock.fillRect(62, 26, 4, 4, SSD1306_WHITE); 
  displayClock.fillRect(62, 42, 4, 4, SSD1306_WHITE); 
  
  displayClock.setCursor(71, 12); 
  displayClock.print(timeMin);

  // 2. Corner Data (Size 1)
  displayClock.setTextSize(1);
  
  // Top Right Corner: AM/PM (Y=2, safely above the Size 5 text)
  displayClock.setCursor(114, 2); 
  displayClock.print(F("PM"));

  // Bottom Right Corner: Seconds only (Y=54, safely below the Size 5 text)
  displayClock.setCursor(114, 54); 
  displayClock.print(timeSec);
  
  displayClock.display();

  // --------------------------------------------------
  // RENDER SCREEN 2: LOCKED LEFT CALENDAR (0x3D)
  // --------------------------------------------------
  displayCalendar.clearDisplay();
  displayCalendar.setTextColor(SSD1306_WHITE);

  // Line 1: Date (Size 3)
  displayCalendar.setTextSize(3);
  displayCalendar.setCursor(0, 2); 
  displayCalendar.print(F("29"));

  // Line 2: "MONDAY" (Size 2)
  displayCalendar.setTextSize(2);
  displayCalendar.setCursor(0, 28);
  displayCalendar.print(F("MONDAY"));

  // Line 3: "JUNE" (Size 2)
  displayCalendar.setCursor(0, 46);
  displayCalendar.print(F("JUNE")); 

  displayCalendar.display();

  delay(1000); 
}