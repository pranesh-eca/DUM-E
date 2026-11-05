#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 displayLeft(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 displayRight(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Non-blocking timer variables
unsigned long lastSecondTick = 0;
unsigned long lastScrollTick = 0;

// Timer State
int totalTimerSeconds = 14 * 60 + 59; // Total starting time (14:59)
int fakeSecondsRemaining = totalTimerSeconds;

// Dynamic Scrolling Variables
int scrollX = 0;
String taskText = "ASIC/VLSI Design Sprint"; 
int taskWidth; 

void setup() {
  Serial.begin(115200);
  
  if(!displayLeft.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  if(!displayRight.begin(SSD1306_SWITCHCAPVCC, 0x3D)) for(;;);

  displayLeft.setRotation(0);
  displayRight.setRotation(0);
  displayLeft.setTextWrap(false);
  displayRight.setTextWrap(false);
  
  // Calculate text width: Size 2 font is 12 pixels wide per character
  taskWidth = taskText.length() * 12; 
  
  if (taskWidth <= 128) {
    scrollX = (128 - taskWidth) / 2; // Lock to center if it fits
  } else {
    scrollX = 128; // Start completely off-screen right if it needs to scroll
  }
}

void loop() {
  unsigned long currentMillis = millis();

  // -------------------------------------------------------------
  // TIME & PHYSICS ENGINE
  // -------------------------------------------------------------
  if (currentMillis - lastSecondTick >= 1000) {
    lastSecondTick = currentMillis;
    if (fakeSecondsRemaining > 0) fakeSecondsRemaining--;
  }

  if (currentMillis - lastScrollTick >= 25) {
    lastScrollTick = currentMillis;
    if (taskWidth > 128) {
      scrollX--;
      if (scrollX < -taskWidth) {
        scrollX = 128; 
      }
    }
  }

  // -------------------------------------------------------------
  // D1: MASTER CLOCK (Always On)
  // -------------------------------------------------------------
  displayLeft.clearDisplay();
  displayLeft.setTextColor(SSD1306_WHITE);
  
  char hourStr[3], minStr[3], secStr[3];
  sprintf(hourStr, "%02d", 10);
  sprintf(minStr, "%02d", 44);
  sprintf(secStr, "%02d", (60 - (fakeSecondsRemaining % 60)) % 60); 

  displayLeft.setTextSize(5); 
  displayLeft.setCursor(2, 12); 
  displayLeft.print(hourStr);
  
  // Master Clock Colons
  displayLeft.fillRect(61, 26, 4, 4, SSD1306_WHITE); 
  displayLeft.fillRect(61, 42, 4, 4, SSD1306_WHITE); 
  
  displayLeft.setCursor(70, 12); 
  displayLeft.print(minStr);
  
  displayLeft.setTextSize(1); 
  displayLeft.setCursor(114, 2); 
  displayLeft.print("AM"); 
  displayLeft.setCursor(114, 54); 
  displayLeft.print(secStr); 
  
  displayLeft.display();

  // -------------------------------------------------------------
  // D2: FOCUS TIMER + PROGRESS BAR + DYNAMIC TEXT
  // -------------------------------------------------------------
  displayRight.clearDisplay();
  displayRight.setTextColor(SSD1306_WHITE);

  int m = fakeSecondsRemaining / 60;
  int s = fakeSecondsRemaining % 60;
  char timerM[3], timerS[3];
  sprintf(timerM, "%02d", m);
  sprintf(timerS, "%02d", s);

  // 1. Countdown Timer (Size 4, Top)
  displayRight.setTextSize(4);
  displayRight.setCursor(6, 4);
  displayRight.print(timerM);
  displayRight.print(":"); 
  displayRight.print(timerS);

  // 2. The Borderless Progress Bar (Middle Gap)
  // Maps the remaining time to a pixel width between 0 and 128
  int barWidth = map(fakeSecondsRemaining, 0, totalTimerSeconds, 0, 128);
  displayRight.fillRect(0, 39, barWidth, 2, SSD1306_WHITE);

  // 3. Task Text (Size 2, Bottom)
  displayRight.setTextSize(2);
  displayRight.setCursor(scrollX, 46);
  displayRight.print(taskText);

  displayRight.display();
}