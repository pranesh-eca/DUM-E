#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 displayLeft(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_SSD1306 displayRight(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define MAX_PARTICLES 15
struct Particle {
  int x;
  float y;
  float speed;
  int length;
  char character;
  int state; 
};
Particle shower[MAX_PARTICLES];

void setup() {
  Serial.begin(115200);
  
  if(!displayLeft.begin(SSD1306_SWITCHCAPVCC, 0x3C)) for(;;);
  if(!displayRight.begin(SSD1306_SWITCHCAPVCC, 0x3D)) for(;;);

  displayLeft.setRotation(0);
  displayRight.setRotation(0);
  displayLeft.setTextWrap(false);
  displayRight.setTextWrap(false);
  
  initShowerParticles(0);
}

void loop() {
  // Cycles only through your 6 chosen optimized showers (4 seconds each)
  runOverlayTest("FLOWERS", 4000);
  runOverlayTest("STRIPES", 4000);
  runOverlayTest("SNOW", 4000);
  runOverlayTest("CROSSES", 4000);
  runOverlayTest("BLOCKS", 4000);
  runOverlayTest("SPEEDLINES", 4000);
}

// =================================================================
// DUMMY BACKGROUND CLOCK
// =================================================================
void drawBackgroundClock() {
  displayLeft.setTextColor(SSD1306_WHITE);
  displayLeft.setTextSize(5);
  displayLeft.setCursor(2, 12); 
  displayLeft.print("12");
  displayLeft.fillRect(62, 26, 4, 4, SSD1306_WHITE); 
  displayLeft.fillRect(62, 42, 4, 4, SSD1306_WHITE); 
  displayLeft.setCursor(71, 12); 
  displayLeft.print("24");
  displayLeft.setTextSize(1);
  displayLeft.setCursor(114, 2); 
  displayLeft.print("AM");
  displayLeft.setCursor(114, 54); 
  displayLeft.print("09");

  displayRight.setTextColor(SSD1306_WHITE);
  displayRight.setTextSize(3);
  displayRight.setCursor(0, 2); 
  displayRight.print("01");
  displayRight.setTextSize(2);
  displayRight.setCursor(0, 28);
  displayRight.print("WEDNESDAY");
  displayRight.setCursor(0, 46);
  displayRight.print("JULY"); 
}

// =================================================================
// OVERLAY RENDER CONSTANTS & INITIALIZATION
// =================================================================
void initShowerParticles(int mode) {
  for(int i = 0; i < MAX_PARTICLES; i++) {
    shower[i].x = random(0, 128);
    shower[i].y = random(-64, 0);
    shower[i].speed = random(12, 38) / 10.0;
    shower[i].length = random(6, 20);
    shower[i].character = random(0, 2) == 0 ? '0' : '1';
    shower[i].state = 0;
    
    // Formatting logic for stripes
    if (mode == 5) { 
      shower[i].x = (i * 9) % 128;
      if (shower[i].x % 2 != 0) shower[i].x++;
    }
  }
}

// =================================================================
// INTERACTION EXECUTION CORE (6 Optimized Styles)
// =================================================================
void runOverlayTest(String style, unsigned long duration) {
  unsigned long startTime = millis();
  
  if (style == "STRIPES") initShowerParticles(5);
  else initShowerParticles(0);

  while(millis() - startTime < duration) {
    long t = millis();
    displayLeft.clearDisplay();
    displayRight.clearDisplay();
    
    drawBackgroundClock();
    
    for(int i = 0; i < MAX_PARTICLES; i++) {
      
      // Simplified physics translation
      if (style == "SPEEDLINES") shower[i].y += (shower[i].speed * 2.5);
      else shower[i].y += shower[i].speed;

      // Out of bounds limit check
      if(shower[i].y > 64) {
        shower[i].y = random(-32, 0);
        shower[i].x = random(0, 128);
        shower[i].speed = random(12, 38) / 10.0;
        shower[i].length = random(6, 20);
        shower[i].character = random(0, 2) == 0 ? '0' : '1';
        shower[i].state = 0;
        
        if (style == "STRIPES") {
          shower[i].x = (i * 9) % 128;
          if (shower[i].x % 2 != 0) shower[i].x++;
        }
      }

      int curX = shower[i].x;
      int curY = (int)shower[i].y;

      // 1. FLOWERS
      if (style == "FLOWERS") {
        int driftX = sin(curY * 0.05 + i) * 3;
        int txL = constrain(curX + driftX, 2, 125);
        int txR = constrain(curX - driftX, 2, 125);
        
        displayLeft.drawPixel(txL, curY, SSD1306_WHITE);
        displayLeft.drawPixel(txL-1, curY, SSD1306_WHITE); displayLeft.drawPixel(txL+1, curY, SSD1306_WHITE);
        displayLeft.drawPixel(txL, curY-1, SSD1306_WHITE); displayLeft.drawPixel(txL, curY+1, SSD1306_WHITE);
        
        displayRight.drawPixel(txR, curY, SSD1306_WHITE);
        displayRight.drawPixel(txR-1, curY, SSD1306_WHITE); displayRight.drawPixel(txR+1, curY, SSD1306_WHITE);
        displayRight.drawPixel(txR, curY-1, SSD1306_WHITE); displayRight.drawPixel(txR, curY+1, SSD1306_WHITE);
      }
      
      // 2. STRIPES
      else if (style == "STRIPES") {
        displayLeft.drawFastVLine(curX, curY, shower[i].length, SSD1306_WHITE);
        displayRight.drawFastVLine(curX, curY, shower[i].length, SSD1306_WHITE);
      }
      
      // 3. SNOW
      else if (style == "SNOW") {
        int driftX = sin(curY * 0.08 + (i * 5)) * 4;
        displayLeft.drawPixel(constrain(curX + driftX, 0, 127), curY, SSD1306_WHITE);
        displayRight.drawPixel(constrain(curX - driftX, 0, 127), curY, SSD1306_WHITE);
      }
      
      // 4. CROSSES
      else if (style == "CROSSES") {
        if ((t / 200 + i) % 2 == 0) {
          displayLeft.drawLine(curX - 2, curY, curX + 2, curY, SSD1306_WHITE);
          displayLeft.drawLine(curX, curY - 2, curX, curY + 2, SSD1306_WHITE);
          displayRight.drawLine(curX - 2, curY, curX + 2, curY, SSD1306_WHITE);
          displayRight.drawLine(curX, curY - 2, curX, curY + 2, SSD1306_WHITE);
        }
      }
      
      // 5. BLOCKS
      else if (style == "BLOCKS") {
        displayLeft.fillRect(curX, curY, 3, 3, SSD1306_INVERSE);
        displayRight.fillRect(curX, curY, 3, 3, SSD1306_INVERSE);
      }
      
      // 6. SPEEDLINES
      else if (style == "SPEEDLINES") {
        displayLeft.drawFastVLine(curX, curY, shower[i].length * 1.5, SSD1306_INVERSE);
        displayRight.drawFastVLine(curX, curY, shower[i].length * 1.5, SSD1306_INVERSE);
      }
    } 

    displayLeft.display();
    displayRight.display();
    delay(15); 
  } 
}