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
//////////////////////////////////////////////////////////////////  
  tft.begin(identifier);
    
  //background
    tft.fillScreen(WHITE);
    delay(2000);

  // set up grid array of 8 x 6 objects
  int grid[6][6];

//  tft.fillCircle(100, 100, 40, GREEN);
//  delay(1000);
  //randomize grid
  
 for (int x = 0; x < 3; x++) {
    for (int y = 0; y < 3; y++) {
      
      grid[x][y] = random(6);
//      Serial.println(grid[x][y]);
    }
 }

/*Numbers corresponding to each colour:
RED = 0
ORANGE = 1
YELLOW = 2
GREEN = 3
BLUE = 4
PURPLE = 5
*/

//Initialization variables
int cSize = 10;
int elementDistance = 40;
int x1Border = elementDistance/2;
//Draw grid
  for (int i = 0; i <= 5; i++) {
    for (int j = 0; j <= 6; j++) {
      tft.drawFastVLine(i*elementDistance + 20, 20, 240, BLACK);
      tft.drawFastHLine(20, j*elementDistance + 20, 200, BLACK);
    }
  }
      
  for (int i = 0; i <= 4; i++) {
    for (int j = 0; j <= 5; j++) {
      // Assign random colours to each circle
      grid[i][j] = random(6);
      if (grid[i][j] == 0) 
        tft.fillCircle((i+1)*elementDistance, (j+1)*elementDistance, cSize, RED);
      else if (grid[i][j] == 1) 
        tft.fillCircle((i+1)*elementDistance, (j+1)*elementDistance, cSize, ORANGE);
      else if (grid[i][j] == 2) 
        tft.fillCircle((i+1)*elementDistance, (j+1)*elementDistance, cSize, YELLOW);
      else if (grid[i][j] == 3) 
        tft.fillCircle((i+1)*elementDistance, (j+1)*elementDistance, cSize, GREEN);
      else if (grid[i][j] == 4) 
        tft.fillCircle((i+1)*elementDistance, (j+1)*elementDistance, cSize, BLUE);
      else if (grid[i][j] == 5)  
        tft.fillCircle((i+1)*elementDistance, (j+1)*elementDistance, cSize, PURPLE);
            
    }
}

      


}

void loop() {
 // TSPoint p = ts.getPoint();
}
