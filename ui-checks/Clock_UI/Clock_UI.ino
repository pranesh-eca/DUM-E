#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// --- Display Configuration ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define i2c_Address 0x3c

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  delay(250);

  if(!display.begin(i2c_Address, true)) {
    Serial.println(F("SH1106 allocation failed. Check wiring!"));
    for(;;); 
  }
  
  display.setRotation(2); 
  display.setTextWrap(false);
  
  drawClockUI();
}

void loop() {
  // Static UI for visual layout verification
}

void drawClockUI() {
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  // --------------------------------------------------
  // 1. MASSIVE TIME DISPLAY (Size 4 = 32px tall)
  // --------------------------------------------------
  display.setTextSize(4);
  
  // Hours 
  display.setCursor(4, 8); 
  display.print(F("10"));

  // Custom Tighter Colon 
  display.fillRect(54, 16, 4, 4, SH110X_WHITE); // Top dot
  display.fillRect(54, 28, 4, 4, SH110X_WHITE); // Bottom dot

  // Minutes 
  display.setCursor(62, 8); 
  display.print(F("35"));

  // --------------------------------------------------
  // 2. AM/PM & SECONDS BLOCK (Right Aligned)
  // --------------------------------------------------
  display.setTextSize(1);
  
  // Superscript AM/PM 
  display.setCursor(113, 8); 
  display.print(F("PM"));

  // Subscript Seconds 
  display.setCursor(113, 32); 
  display.print(F("59"));

  // --------------------------------------------------
  // 3. DATE ROW (Bottom Center - Custom Pixel Spacing)
  // --------------------------------------------------
  display.setTextSize(2);
  
  // Print "28"
  display.setCursor(7, 48); 
  display.print(F("28"));

  // Print "JUN" with a tighter 6px gap (Starts at X=37 instead of X=43)
  display.setCursor(37, 48); 
  display.print(F("JUN"));

  // Custom 2px Comma tucked right next to the "N"
  display.fillRect(74, 60, 2, 2, SH110X_WHITE); // Comma body
  display.drawPixel(73, 62, SH110X_WHITE);      // Comma tail

  // Print "SUN" with a wider 10px gap
  display.setCursor(86, 48); 
  display.print(F("SUN"));

  // Render to screen
  display.display();
}
