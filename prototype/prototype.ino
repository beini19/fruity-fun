#include <Elegoo_GFX.h> //include libraries
#include <Elegoo_TFTLCD.h>
#include <TouchScreen.h>

//pins
#define CS A3 
#define CD A2 
#define WR A1
#define RD A0 
#define RESET A4 

Elegoo_TFTLCD tft(CS, CD, WR, RD, RESET);

//assign readable names to 16-bit RGB565 colour variables
#define RED     0xF9C7
#define GREEN   0x7774
#define YELLOW  0xFF93
#define BLUE    0x755E
#define ORANGE  0xF5F0
#define PURPLE  0xCC1D
#define BLACK   0x0000
#define WHITE   0xFFFF

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

void setup() {
  Serial.begin(9600);

  tft.reset();
    uint16_t identifier = tft.readID();
  if(identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  }else if(identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if(identifier==0x0101)
  {     
      identifier=0x9341;
       Serial.println(F("Found 0x9341 LCD driver"));
  }else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier=0x9341;
   
  }
    tft.begin(identifier);
    
  //background
    tft.fillScreen(BLACK);
    delay(2000);

  // set up grid array of 8 x 6 objects
  int grid[6][6];
  tft.fillCircle(100, 100, 40, RED);
  delay(1000);
  //randomize grid

  for (int x = 0; x < 3; x ++)
    for (int y = 0; y < 3; y--) {
      grid[x][y] = random(6);
      if (grid[x][y] == 0)
        tft.fillCircle(x*10, 100, 10, RED);
      else if (grid[x][y] == 1)
         tft.fillCircle(40, 200, 10, ORANGE);
      else if (grid[x][y] == 2)
         tft.fillCircle(x*50, y*100, 10, YELLOW);
      else if (grid[x][y] == 3)
         tft.fillCircle(x*50, y*100, 10, GREEN);
      else if (grid[x][y] == 4)
        tft.fillCircle(50, y*100, 10, BLUE);
      else
          tft.fillRect(300, 130, 5, 10, PURPLE);
    }
  delay(1000);
}
/*
 * void (
 */
void loop() {
 // TSPoint p = ts.getPoint();
}
