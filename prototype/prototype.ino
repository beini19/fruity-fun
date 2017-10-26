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
    tft.fillScreen(BLACK);
    delay(2000);

  // set up grid array of 5 x 6 objects
  int grid[5][6];

//  tft.fillCircle(100, 100, 40, GREEN);
//  delay(1000);
  //randomize grid
  
 for (int x = 0; x < 5; x++) {
    for (int y = 0; y < 6; y++) {
      
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
#define cSize 10
#define elementDistance 40
#define x1Border 20;

#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define MINPRESSURE 10
#define MAXPRESSURE 1000
  

//Draw grid
  for (int i = 0; i <= 5; i++) {
      tft.drawFastVLine(i*elementDistance + 20, 20, 240, WHITE);
  }
  for (int j = 0; j <= 6; j++) {
      tft.drawFastHLine(20, j*elementDistance + 20, 200, WHITE);
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

 
}//end of setup


void loop() {

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  bool isTouched[5][6] = {false}; //Boolean values are true when area of screen is being pressed
  
  //Redraw elements on screen everytime user touches something? Probably a bad idea, find some way to make 'grid' a global variable
 
  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
    
    // scale from 0->1023 to tft.width
    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
    p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
//    bool isTouching(void);

int selectElement = 0;

//Check each grid to see if it's been pressed
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 6; j++) {
      if (p.x > (20 + elementDistance*i) && p.x < (20 + elementDistance*(i+1))
       && p.y > (40 + elementDistance*j) && p.y < (40 + elementDistance*(j+1))) {
          isTouched[i][j] = true;
          selectElement++;
          if (selectElement <= 2) {
           tft.drawRect(tft.width() - (20 + elementDistance*(i+1)),tft.height() - (20 + elementDistance*(j+2)), 
           elementDistance + 1, elementDistance + 1, RED);
          }
          else if (selectElement > 2) {
              for (int i = 0; i <= 5; i++) {
                 tft.drawFastVLine(i*elementDistance + 20, 20, 240, WHITE);
              }
              for (int j = 0; j <= 6; j++) {
                  tft.drawFastHLine(20, j*elementDistance + 20, 200, WHITE);
               }      
             }
           }
        }
      
    }
  }
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 6; j++) {
      if (isTouched[i][j]) {
        Serial.print("Grid "); Serial.print(i); Serial.print(" "); Serial.println(j);  //Print grid
        
        //Highlight the grid that's been pressed
        
    }
  }
       
  }
}
