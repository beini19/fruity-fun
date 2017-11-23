#include <Elegoo_GFX.h> //include libraries
#include <Elegoo_TFTLCD.h>
#include <SD.h>
#include <TouchScreen.h>

//Import C libraries
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <time.h>
#include <math.h>

//pins
#define CS A3
#define CD A2
#define WR A1
#define RD A0
#define RESET A4
#define PIN_SD_CS 10 

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

//SD variables
#define MAX_BMP         10                     
#define FILENAME_LEN    20 

#define BUFFPIXEL       2             //pixels to be read at a time
#define BUFFPIXEL_X3    6             //buffpixel * 3

int bmp_height = 30;                 // bmp height
int bmp_width = 30;                 // bmp width
unsigned char bmp_image_offset  = 54;        // offset

int bmp_file_num = 6;  
char bmp_files[6][FILENAME_LEN] =      
{
"apple.bmp", //0
"orange.bmp", //1
"banana.bmp", //2
"pear.bmp", //3
"blue.bmp", //3
"cherries.bmp", //5
};

File bmpFile; 

#define BUTTON_X 40
#define BUTTON_Y 290
#define BUTTON_W 60
#define BUTTON_H 30
#define BUTTON_SPACING_X 15
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2

#define ROWS 5
#define COLUMNS 6
int moves = 20;
int x1 = -1;
int x2 = -1;
int y1 = -1;
int y2 = -1;
//int previousGridX;
//int previousGridY;
int selectElement = 0;

int grid[5][6];
int points = 0;
int countMatches[6] = {0};
bool possible;
int matched[ROWS][COLUMNS] = {0};
int posMove[ROWS][COLUMNS] = {0};

Elegoo_GFX_Button menuButton;
Elegoo_GFX_Button resetButton;
Elegoo_GFX_Button closeMenuButton;
bool menuOpen = false;

/************************  FUNCTIONS  **************************************************/
void bmpdraw(File f, int x, int y) //draws bitmap to screen
{
    bmpFile.seek(bmp_image_offset);

    int rowsize = ((30*3+3) & -3);   //accomodate for padding

          Serial.println("huhafsdf");
    uint8_t sdbuffer[BUFFPIXEL_X3];                 // 3 * pixels to buffer
                 
    for (int i=0; i< bmp_height; i+=3) {
        for(int j=0; j<(rowsize/BUFFPIXEL); j++) {
            
            bmpFile.read(sdbuffer, BUFFPIXEL_X3); //read in data from bmp file

            uint8_t buffidx = 0;
            int offset_x = (j*BUFFPIXEL);
            unsigned int __color[BUFFPIXEL];
            
            for(int k=0; k<BUFFPIXEL; k++) {
                __color[k] = sdbuffer[buffidx+2]>>3;                        // red
                __color[k] = __color[k]<<6 | (sdbuffer[buffidx+1]>>2);      // green
                __color[k] = __color[k]<<5 | (sdbuffer[buffidx+0]>>3);      // blue
                //Serial.println(buffidx);
                buffidx += 3;
            }

      for (int m = 0; m < BUFFPIXEL; m ++) { //print 2 pixels at a time on row

              if(j < 16) { //otherwise we get three images for some reason
              tft.drawPixel(x+ m+offset_x, y - i,__color[m]);
              tft.drawPixel(x+ m+offset_x, y - i+1,__color[m]);
              tft.drawPixel(x+ m+offset_x, y - i+2,__color[m]);
              }
         }
      }
    }
}

void reset(int grid[ROWS][COLUMNS]) {//reset every element of grid to 0
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      grid[i][j] = 8;//no element would ever equal 8 under any circumstances
    }
  }
}


void printNumGrid(int grid[ROWS][COLUMNS]) {
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      if (grid[x][y] < 0) {
        Serial.print(grid[x][y]); Serial.print(" ");
      } else {
        Serial.print(" "); Serial.print(grid[x][y]); Serial.print(" ");
      }
    }
    Serial.println();
  }
}

//Draw fruit from bmp files
//Animate grid: have printGrid take in a changed array idicating changed values
//If changed[][] is true, make for loop with row number as end condition, draw element at row[i]
void printGrid(int grid[ROWS][COLUMNS]) {
  for (int x = 0; x < ROWS; x ++) {
    for (int y = 0; y < COLUMNS; y++) {
      //Check the random number to determine which colour fruit to draw
      if (grid[x][y] >= 0 && grid[x][y] <=5) {
        bmpFile = SD.open(bmp_files[grid[x][y]]);
        bmpdraw(bmpFile, (x)*elementDistance+25, (COLUMNS - y)*elementDistance+15);
        bmpFile.close();
             /* Fruit
       *  0 = apple
       *  1 = orange
       *  2 = banana
       *  3 = pear 
       *  4 = blueberry/blue
       *  5 = cherries
       */
 
    }
  }
  delay(100);
}
}

int displayMatch(int matchedElement) {
  switch (matchedElement) {
    case 0:
      return -6;
    case 1:
      return -1;
    case 2:
      return -2;
    case 3:
      return -3;
    case 4:
      return - 4;
    case 5:
      return - 5;
  }
}

void countPoints(int matched[ROWS][COLUMNS], int countMatches[6]) {
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      switch (matched[x][y]) {
        case -1:
          countMatches[1]++;
          break;
        case -2:
          countMatches[2]++;
          break;
        case -3:
          countMatches[3]++;
          break;
        case -4:
          countMatches[4]++;
          break;
        case -5:
          countMatches[5]++;
          break;
        case -6:
          countMatches[0]++;
          break;
      }
    }
  }

  for (int i = 0; i < 6; i++) {
    points += countMatches[i] * 10;
    //Display new score
    tft.fillRect(159, 289, 100, 40, BLACK); //x_corner, y_corner, length, width, colour
    tft.setCursor(160, 290);
    tft.setTextColor(WHITE);  tft.setTextSize(2);
    tft.println(points);
    countMatches[i] = 0;
  }

  reset(matched);
}

bool checkGrid (int grid[ROWS][COLUMNS], int matched[ROWS][COLUMNS]) { //perhaps replace parameters bc of header?
  bool change = false;
  int match = 0, r = -1;

  //check rows for matches
  for (int x = 0; x < ROWS; x ++) {
    for (int y = 0; y < COLUMNS; y ++) {
      if (grid[x][y] == r) {
        match++;
      } else {
        match = 0;
      }

      if (match == 2) { //if two matches = 3 in a row
        change = true;
        for (int n = y; n > y - 3; n--) {
    //      matched[x][n] = displayMatch(grid[x][n]);//matched elements are displayed as their corresponding match value
        }
      }
      else if (match > 2) {
     //   matched[x][y] = displayMatch(grid[x][y]);
      }

      r = grid[x][y];
    }
    r = -1;
    match = 0;
  }

  //repeat above code for columns
  for (int y = 0; y < COLUMNS; y ++) {
    for (int x = 0; x < ROWS; x ++) {
      if (grid[x][y] == r) {
        match++;
      } else {
        match = 0;
      }

      if (match == 2) {
        change = true;
        for (int n = x; n > x - 3; n--) {
       //   matched[n][y] = displayMatch(grid[n][y]);
        }
      }
      else if (match > 2) {
      //  matched[x][y] = displayMatch(grid[x][y]);
      }

      r = grid[x][y];
    }
    r = -1;
    match = 0;
  }

  Serial.println("matched: ");
  printNumGrid(matched);
  delay(200);
  return change;
}

//Deletes matches, moves elements down, and generates new elements
void updateGrid(int grid[ROWS][COLUMNS], int matched[ROWS][COLUMNS]) {
  //countPoints(matched);

  //delete matches
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      if (matched[x][y] < 0) {
        grid[x][y] = 8;
        //matches are highlighted in green
        tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x)), tft.height() - (20 + elementDistance * (y + 2)),//i + 1
                     elementDistance + 1, elementDistance + 1, GREEN);
      }
    }
  }
  delay(500);

  Serial.println("matches: ");
  Serial.println("grid: ");
  printGrid(grid);
  printNumGrid(grid);

  //unhighlight where matches used to be
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      if (matched[x][y] < 0) {
        tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x)), tft.height() - (20 + elementDistance * (y + 2)),//i + 1
                     elementDistance + 1, elementDistance + 1, WHITE);
      }
    }
  }

  //shift down
  for (int x = ROWS - 1; x >= 0; x--) {//starting from the bottom row
    for (int y = 0; y < COLUMNS; y++) {//starting from the left-most column
      if (grid[x][y] == 8) {
        for (int a = x - 1; a >= 0; a--) {
          if (grid[a][y] != 8) {
            grid[x][y] = grid[a][y];
            grid[a][y] = 8;
            break;
          }
          delay (100);
        }
      }
    }
  }

  Serial.println("after shifting down\n");
  printNumGrid(grid);

  //refill
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      if (grid[x][y] == 8) {
        grid[x][y] = random(6);
      }
    }
  }

  Serial.println("after refilling\n");
  printGrid(grid);
  printNumGrid(grid);

//  countPoints(matched, countMatches);
  Serial.print("total points: "); Serial.println(points);

  //check new grid for matches
  if (checkGrid(grid, matched)) {
    updateGrid(grid, matched);
  }
}

void shuffle(int grid[ROWS][COLUMNS]) {
  Serial.println("\nNo possible moves: shuffling\n");
  for (int x = 0; x < ROWS; x ++) {
    for (int y = 0; y < COLUMNS; y++) {
      grid[x][y] = rand() % 6;
    }
  }
  printGrid(grid);
}

bool movePossible(int grid[ROWS][COLUMNS], int posMove[ROWS][COLUMNS]) {
  int a = 0;
  bool isPossible = false;

  for (int x = 0; x < ROWS; x++) {//checks from left to right
    for (int y = 0; y < COLUMNS; y++) {//checks from top to bottom
      a = grid[x][y];

      //check rows
      if (y + 1 < COLUMNS && grid[x][y + 1] == a) {
        if (y - 2 >= 0 && grid[x][y - 2] == a) {
          isPossible = true;
          posMove[x][y - 2] = 6; //elements that could be matched are indicated with 6
        } else if (y - 1 >= 0 && x - 1 >= 0 && grid[x - 1][y - 1] == a) {
          isPossible = true;
          posMove[x - 1][y - 1] = 6;
        } else if (y - 1 >= 0 && x + 1 < ROWS && grid[x + 1][y - 1] == a) {
          isPossible = true;
          posMove[x + 1][y - 1] = 6;
        } else if (y + 2 < COLUMNS && x - 1 >= 0 && grid[x - 1][y + 2] == a) {
          isPossible = true;
          posMove[x - 1][y + 2] = 6;
        } else if (y + 2 < COLUMNS && x + 1 < ROWS && grid[x + 1][y + 2] == a) {
          isPossible = true;
          posMove[x + 1][y + 2] = 6;
        } else if (y + 3 < COLUMNS && grid[x][y + 3] == a) {
          isPossible = true;
          posMove[x][y + 3] = 6;
        }
      }

      if (isPossible) {
        posMove[x][y] = 6;
        posMove[x][y + 1] = 6;
        return isPossible;
      }

      //check columns
      if (x + 1 < ROWS && grid[x + 1][y] == a) {
        if (x - 2 >= 0 && grid[x - 2][y] == a) {
          isPossible = true;
          posMove[x - 2][y] = 6;
        } else if (x - 1 >= 0 && y - 1 >= 0 && grid[x - 1][y - 1] == a) {
          isPossible = true;
          posMove[x - 1][y - 1] = 6;
        } else if (x - 1 >= 0 && y + 1 < COLUMNS && grid[x - 1][y + 1] == a) {
          isPossible = true;
          posMove[x - 1][y + 1] = 6;
        } else if (x + 2 < ROWS && y - 1 >= 0 && grid[x + 2][y - 1] == a) {
          isPossible = true;
          posMove[x + 2][y - 1] = 6;
        } else if (x + 2 < ROWS && y + 1 < COLUMNS && grid[x + 2][y + 1] == a) {
          isPossible = true;
          posMove[x + 2][y + 1] = 6;
        } else if (x + 3 < ROWS && grid[x + 3][y] == a) {
          isPossible = true;
          posMove[x + 3][y] = 6;
        }
      }

      if (isPossible) {
        posMove[x][y] = 6;
        posMove[x + 1][y] = 6;
        return isPossible;
      }
    }
  }

  return isPossible;//if none of the previous conditions were satisfied, no move is possible
}

void switchValues (int grid[ROWS][COLUMNS], int x1, int y1, int x2, int y2) {
  Serial.println("switching values...");

  //switch two values in grid
  int r;

  printNumGrid(grid);
  Serial.print("x1: "); Serial.println(x1); Serial.print(" y1: "); Serial.println(y1); Serial.print(" x2: "); Serial.println(x2); Serial.print(" y2: "); Serial.println(y2);
  Serial.print("grid[x1][y1]: "); Serial.println(grid[x1][y1]);
  Serial.print("grid[x2][y2]: "); Serial.println(grid[x2][y2]);

  //switch values
  r = grid[x1][y1];
  grid[x1][y1] = grid[x2][y2];
  grid[x2][y2] = r;

  Serial.println("After switching values: ");
  //Serial.print("x1: "); Serial.println(x1); Serial.print("y1: "); Serial.println(y1); Serial.print("x2: "); Serial.println(x2); Serial.print("y2: "); Serial.println(y2);
  Serial.print("grid[x1][y1]: "); Serial.println(grid[x1][y1]);
  Serial.print("grid[x2][y2]: "); Serial.println(grid[x2][y2]);
  printGrid(grid);
  Serial.println("After printing grid");
  printNumGrid(grid);
}

/*********************************  SETUP  *****************************************/

void setup() {
  Serial.begin(9600);

#ifdef USE_Elegoo_SHIELD_PINOUT
  Serial.println(F("Using Elegoo 2.4\" TFT Arduino Shield Pinout"));
#else
  Serial.println(F("Using Elegoo 2.4\" TFT Breakout Board Pinout"));
#endif


  tft.reset();
  uint16_t identifier = tft.readID();
  if (identifier == 0x9325) {
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if (identifier == 0x9328) {
    Serial.println(F("Found ILI9328 LCD driver"));
  } else if (identifier == 0x4535) {
    Serial.println(F("Found LGDP4535 LCD driver"));
  } else if (identifier == 0x7575) {
    Serial.println(F("Found HX8347G LCD driver"));
  } else if (identifier == 0x9341) {
    Serial.println(F("Found ILI9341 LCD driver"));
  } else if (identifier == 0x8357) {
    Serial.println(F("Found HX8357D LCD driver"));
  } else if (identifier == 0x0101)
  {
    identifier = 0x9341;
    Serial.println(F("Found 0x9341 LCD driver"));
  } else {
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.println(F("If using the Elegoo 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_Elegoo_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Elegoo_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    identifier = 0x9341;

  }

  randomSeed(analogRead(0));
  tft.begin(identifier);
  
  //Inititiate SD_Card
  pinMode(10, OUTPUT);

    if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    tft.setCursor(0, 0);
    tft.setTextColor(WHITE);    
    tft.setTextSize(1);
    tft.println("SD Card Init fail.");   
  }else
  Serial.println("initialization done.");
  
  //background
  tft.fillScreen(BLACK);
  delay(2000);

  //Draw menu button
  menuButton.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, WHITE,
                        BLUE, WHITE, "Menu", BUTTON_TEXTSIZE);  // x, y, w, h, outline, fill, text
  menuButton.drawButton();

  //Draw reset button
  resetButton.initButton(&tft, BUTTON_X + BUTTON_W + BUTTON_SPACING_X, BUTTON_Y, BUTTON_W, BUTTON_H, WHITE,
                         BLUE, WHITE, "Reset", BUTTON_TEXTSIZE);  // x, y, w, h, outline, fill, text
  resetButton.drawButton();


  //Print score/points to screen
  tft.setCursor(160, 270);
  tft.setTextColor(WHITE);  tft.setTextSize(2);
  tft.println("Score:");

  tft.setCursor(160, 290);
  tft.setTextColor(WHITE);  tft.setTextSize(2);
  tft.println(points);

  //Draw gridlines
  for (int i = 0; i <= 5; i++) {
    tft.drawFastVLine(i * elementDistance + 20, 20, 240, WHITE);
  }
  for (int j = 0; j <= 6; j++) {
    tft.drawFastHLine(20, j * elementDistance + 20, 200, WHITE);
  }

  //Set up initial candy grid
  for (int x = 0; x < ROWS; x ++) {
    for (int y = 0; y < COLUMNS; y++) {
      grid[x][y] = random(6);
    }
  }
  Serial.println("original grid: ");
  printGrid(grid);
  printNumGrid(grid);

  //set everything in matched to 8
//  reset(matched);


  //set everything in posMove to 8
//  reset(posMove);

  //checks original grid for matches and updates
  if (checkGrid(grid, matched)) {
    Serial.println("there are matches!");
    printGrid(matched);
    Serial.println("matched: ");
    printNumGrid(matched);
    updateGrid(grid, matched);
    Serial.println("updated grid!");
    printGrid(grid);
    printNumGrid(grid);
  }

  //checks if match-free grid has any possible moves
//  possible = movePossible(grid, posMove);
//  Serial.print("is move possible? ");
//  Serial.println(possible);
//  printGrid(posMove);
//  printNumGrid(posMove);
//  printGrid(grid);
//  while (!possible) {
//    Serial.println("move is not possible :(");
//    shuffle(grid);

//    if (checkGrid(grid, matched)) {
//      Serial.println("there are matches!");
//      printGrid(matched);
//      updateGrid(grid, matched);
//      Serial.println("updated grid");
//    }
//    Serial.println("after checking for matches: ");
//    printGrid(grid);
//
////    reset(posMove);
//    possible = movePossible(grid, posMove);
//    Serial.print("is move possible? ");
//    Serial.println(possible);
//    Serial.println("possible moves: ");
//    printGrid(posMove);
//  }
  }
 
/********************************************** END OF SETUP *******************************************/

  void loop() {

  digitalWrite(13, HIGH);
  TSPoint p = ts.getPoint();
  digitalWrite(13, LOW);

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

//  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
//
//
//    // scale from 0->1023 to tft.width
//    p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
//    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
//    if (!menuOpen) {
//      // Check each grid to see if it's been pressed
//      for (int i = 0; i < 5; i++) {
//        for (int j = 0; j < 6; j++) {
//          if (p.x > (20 + elementDistance * (ROWS - i - 1)) && p.x < (20 + elementDistance * (ROWS - i))
//              && p.y > (40 + elementDistance * j) && p.y < (40 + elementDistance * (j + 1))) {
//            isTouched[i][j] = true;
//            //Serial.print("i: "); Serial.print(i); Serial.print(" j: "); Serial.print(j);
//            if (i != x1 || j != y1) {//a different element is selected
//              selectElement++;
//              //Serial.println("increasing selectElement!");
//            }
//
//            Serial.print("x1: "); Serial.print(x1); Serial.print(" y1: "); Serial.println(y1);
//            Serial.print("x2: "); Serial.print(x2); Serial.print(" y2: "); Serial.println(y2);
//
//            if (selectElement == 1) {
//              //Serial.println("in selectElement == 1");
//              x1 = i;
//              y1 = j;
//              tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - i)), tft.height() - (20 + elementDistance * (j + 2)),//i + 1
//                           elementDistance + 1, elementDistance + 1, RED);
//              Serial.print("Grid "); Serial.print(x1); Serial.print(" "); Serial.println(y1);  //Print grid
//
//            } else if (selectElement == 2) {
//              //Serial.println("in selectElement == 2");
//              x2 = i;
//              y2 = j;
//
//              Serial.print("x1: "); Serial.print(x1); Serial.print(" y1: "); Serial.println(y1);
//              Serial.print("x2: "); Serial.print(x2); Serial.print(" y2: "); Serial.println(y2);
//
//              tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - i)), tft.height() - (20 + elementDistance * (j + 2)),
//                           elementDistance + 1, elementDistance + 1, RED);
//              Serial.print("Grid "); Serial.print(x2); Serial.print(" "); Serial.println(y2);  //Print grid
//
//              if (x1 == x2 || y1 == y2) {//switch must happen in the same row or the same column to be valid
//                switchValues(grid, x1, y1, x2, y2);
//
//                Serial.println("squares should be reverting to white");
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x1)), tft.height() - (20 + elementDistance * (y1 + 2)),
//                             elementDistance + 1, elementDistance + 1, WHITE);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x2)), tft.height() - (20 + elementDistance * (y2 + 2)),
//                             elementDistance + 1, elementDistance + 1, WHITE);
//
//                do {
//                  if (checkGrid(grid, matched)) {
//                    Serial.println("there are matches!");
//                    printGrid(matched);
//                    updateGrid(grid, matched);
//                    Serial.println("updated grid");
//                  }
//                  Serial.println("after checking for matches: ");
//                  printGrid(grid);
//                  printNumGrid(grid);
//
//                  possible = movePossible(grid, posMove);
//                  Serial.print("is move possible? ");
//                  Serial.println(possible);
//                  Serial.println("possible moves: ");
//                  printGrid(posMove);
//                  printNumGrid(posMove);
////                  reset(posMove);
//
//                  if (!possible) {
//                    Serial.println("there are no possible moves :(");
//                    shuffle(grid);
//                  }
//
//                  Serial.println("waiting for input:");
//                } while (!possible);
//
//              } else {
//                Serial.println("Error: switch not possible ");
//                //elements which cannot be matched flash red and white
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x1)), tft.height() - (20 + elementDistance * (y1 + 2)),
//                             elementDistance + 1, elementDistance + 1, WHITE);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x2)), tft.height() - (20 + elementDistance * (y2 + 2)),
//                             elementDistance + 1, elementDistance + 1, WHITE);
//                delay(100);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x1)), tft.height() - (20 + elementDistance * (y1 + 2)),
//                             elementDistance + 1, elementDistance + 1, RED);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x2)), tft.height() - (20 + elementDistance * (y2 + 2)),
//                             elementDistance + 1, elementDistance + 1, RED);
//                delay(100);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x1)), tft.height() - (20 + elementDistance * (y1 + 2)),
//                             elementDistance + 1, elementDistance + 1, WHITE);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x2)), tft.height() - (20 + elementDistance * (y2 + 2)),
//                             elementDistance + 1, elementDistance + 1, WHITE);
//                delay(100);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x1)), tft.height() - (20 + elementDistance * (y1 + 2)),
//                             elementDistance + 1, elementDistance + 1, RED);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x2)), tft.height() - (20 + elementDistance * (y2 + 2)),
//                             elementDistance + 1, elementDistance + 1, RED);
//                delay(100);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x1)), tft.height() - (20 + elementDistance * (y1 + 2)),
//                             elementDistance + 1, elementDistance + 1, WHITE);
//                tft.drawRect(tft.width() - (20 + elementDistance * (ROWS - x2)), tft.height() - (20 + elementDistance * (y2 + 2)),
//                             elementDistance + 1, elementDistance + 1, WHITE);
//              }
//
//            } else if (selectElement > 2) {
//              Serial.println("In selectElement > 2");
//              selectElement = 0;
//              tft.drawRect(tft.width() - (20 + elementDistance * (x1 + 1)), tft.height() - (20 + elementDistance * (y1 + 2)),
//                           elementDistance + 1, elementDistance + 1, WHITE);
//              tft.drawRect(tft.width() - (20 + elementDistance * (x2 + 1)), tft.height() - (20 + elementDistance * (y2 + 2)),
//                           elementDistance + 1, elementDistance + 1, WHITE);
//            }
//          }
//        }
//
//      }
//    }
//
//    //Check if buttons have been pressed
//    
//    if (menuButton.contains((tft.width() - p.x), (tft.height() - p.y))) {
//      //    Serial.print(""); Serial.println("menuButton");
//      menuButton.press(true);  // tell the button it is pressed
//    } else {
//      menuButton.press(false);  // tell the button it is NOT pressed
//    }
//
//    if (menuButton.justReleased() && menuOpen == false) {
//      menuButton.drawButton();  // draw normal
//    } else if (menuButton.justPressed() && menuOpen == false) {
//      Serial.print("Pressing: "); Serial.println("menuButton");
//      menuButton.drawButton(true);  // draw invert
//      //      delay(100);
//      //      menuButton.drawButton();  // draw normal
//
//
//      //Display menu
//      tft.fillScreen(BLACK);
//
//      //Draw closeMenu Button
//      closeMenuButton.initButton(&tft, tft.width() - BUTTON_X, tft.height() - BUTTON_Y, BUTTON_W, BUTTON_H, WHITE,
//                                 BLUE, WHITE, "Close", BUTTON_TEXTSIZE);  // x, y, w, h, outline, fill, text
//      closeMenuButton.drawButton();
//      menuOpen = true;
//      delay(100); // UI debouncing
//    }
//
//    //Read input from closeMenuButton
//    if (closeMenuButton.contains(tft.width() - p.x, tft.height() - p.y) && menuOpen) {
//      Serial.print("Pressing: "); Serial.println("closeMenuButton");
//      closeMenuButton.press(true);  // tell the button it is pressed
//    } else {
//      closeMenuButton.press(false);  // tell the button it is NOT pressed
//    }
//
//    if (closeMenuButton.justReleased() && menuOpen) {
//      closeMenuButton.drawButton();  // draw normal
//    }
//
//    if (closeMenuButton.justPressed() && menuOpen) {
//      closeMenuButton.drawButton(true);  // draw invert
//      menuOpen = false;
//
//      //Redraw game screen:
//      tft.fillScreen(BLACK);
//      //Draw menu button
//      menuButton.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, WHITE,
//                            BLUE, WHITE, "Menu", BUTTON_TEXTSIZE);  // x, y, w, h, outline, fill, text
//      menuButton.drawButton();
//
//      //Draw reset button
//      resetButton.initButton(&tft, BUTTON_X + BUTTON_W + BUTTON_SPACING_X, BUTTON_Y, BUTTON_W, BUTTON_H, WHITE,
//                             BLUE, WHITE, "Reset", BUTTON_TEXTSIZE);  // x, y, w, h, outline, fill, text
//      resetButton.drawButton();
//
//      //Print score/points to screen
//      tft.setCursor(160, 270);
//      tft.setTextColor(WHITE);  tft.setTextSize(2);
//      tft.println("Score:");
//
//      tft.setCursor(160, 290);
//      tft.setTextColor(WHITE);  tft.setTextSize(2);
//      tft.println(points);
//      for (int i = 0; i <= 5; i++) {
//        tft.drawFastVLine(i * elementDistance + 20, 20, 240, WHITE);
//      }
//      for (int j = 0; j <= 6; j++) {
//        tft.drawFastHLine(20, j * elementDistance + 20, 200, WHITE);
//      }
//      printGrid(grid);
//    }
//
//    //Read input from reset Button
//
//    if (resetButton.contains(tft.width() - p.x, tft.height() - p.y)) {
//      Serial.print("Pressing: "); Serial.println("resetButton");
//      resetButton.press(true);  // tell the button it is pressed
//    } else {
//      resetButton.press(false);  // tell the button it is NOT pressed
//    }
//
//    if (resetButton.justReleased()) {
//      resetButton.drawButton();  // draw normal
//    }
//
//    if (resetButton.justPressed()) {
//      resetButton.drawButton(true);  // draw invert
//
//      //Set variables to original values
//      points = 0;
//   //   reset(matched);
//
//      //Redraw game screen:
//      tft.fillScreen(BLACK);
//
//      //Draw menu button
//      menuButton.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H, WHITE,
//                            BLUE, WHITE, "Menu", BUTTON_TEXTSIZE);  // x, y, w, h, outline, fill, text
//      menuButton.drawButton();
//
//      //Draw reset button
//      resetButton.initButton(&tft, BUTTON_X + BUTTON_W + BUTTON_SPACING_X, BUTTON_Y, BUTTON_W, BUTTON_H, WHITE,
//                             BLUE, WHITE, "Reset", BUTTON_TEXTSIZE);  // x, y, w, h, outline, fill, text
//      resetButton.drawButton();
//      //reset game
//      //Print score/points to screen
//      tft.setCursor(160, 270);
//      tft.setTextColor(WHITE);  tft.setTextSize(2);
//      tft.println("Score:");
//
//      tft.setCursor(160, 290);
//      tft.setTextColor(WHITE);  tft.setTextSize(2);
//      tft.println(points);
//      for (int i = 0; i <= 5; i++) {
//        tft.drawFastVLine(i * elementDistance + 20, 20, 240, WHITE);
//      }
//      for (int j = 0; j <= 6; j++) {
//        tft.drawFastHLine(20, j * elementDistance + 20, 200, WHITE);
//      }
//      //Set up initial candy grid
//      for (int x = 0; x < ROWS; x++) {
//        for (int y = 0; y < COLUMNS; y++) {
//          grid[x][y] = random(6);
//        }
//      }
//
//      //Need to get program to recheck matches after array is randomized - currently not working
//      printGrid(grid);
////      reset(posMove);
//      if (checkGrid(grid, matched)) {
//        Serial.println("there are matches!");
//        printGrid(matched);
//        updateGrid(grid, matched);
//        Serial.println("updated grid");
//      }
//      Serial.println("after checking for matches: ");
//      printGrid(grid);
//      printNumGrid(grid);
//
//      possible = movePossible(grid, posMove);
//      Serial.print("is move possible? ");
//      Serial.println(possible);
//      Serial.println("possible moves: ");
//      printGrid(posMove);
//      printNumGrid(posMove);
////      reset(posMove);
//
//      if (!possible) {
//        Serial.println("there are no possible moves :(");
//        shuffle(grid);
//      }
//
//
//    }
    delay(100);
  }
//}



