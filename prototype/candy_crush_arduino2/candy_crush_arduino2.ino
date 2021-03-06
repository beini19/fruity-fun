#include <Dns.h>
#include <Ethernet.h>
#include <EthernetClient.h>
#include <Dhcp.h>
#include <EthernetServer.h>
#include <EthernetUdp.h>

#include <FirmataDefines.h>
#include <Boards.h>
#include <FirmataMarshaller.h>
#include <FirmataParser.h>
#include <FirmataConstants.h>
#include <Firmata.h>

#include <Elegoo_GFX.h> //include libraries
#include <Elegoo_TFTLCD.h>
#include <TouchScreen.h>

//Import C libraries
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
//#include <time.h>
#include <math.h>

int points = 0;
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
int totalPoints = 0;
int countMatches[6] = {0};
bool possible;
int matched[ROWS][COLUMNS] = {0};
int posMove[ROWS][COLUMNS] = {0};
/*Numbers corresponding to each colour:
  RED = 0
  ORANGE = 1
  YELLOW = 2
  GREEN = 3
  BLUE = 4
  PURPLE = 5
*/

/************************  FUNCTIONS  **************************************************/
void reset(int grid[ROWS][COLUMNS]) {//reset every element of grid to 0
  Serial.println("resetting\n");
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLUMNS; j++) {
      grid[i][j] = -1;
    }
  }
}


//Draw candies
void printGrid(int grid[ROWS][COLUMNS]) {
  for (int x = 0; x < ROWS; x ++) {
    for (int y = 0; y < COLUMNS; y++) {
      //Check the random number to determine which colour fruit to draw
      if (grid[x][y] == 0)
        tft.fillCircle((x + 1)*elementDistance, (y + 1)*elementDistance, cSize, RED);
      else if (grid[x][y] == 1)
        tft.fillCircle((x + 1)*elementDistance, (y + 1)*elementDistance, cSize, ORANGE);
      else if (grid[x][y] == 2)
        tft.fillCircle((x + 1)*elementDistance, (y + 1)*elementDistance, cSize, YELLOW);
      else if (grid[x][y] == 3)
        tft.fillCircle((x + 1)*elementDistance, (y + 1)*elementDistance, cSize, GREEN);
      else if (grid[x][y] == 4)
        tft.fillCircle((x + 1)*elementDistance, (y + 1)*elementDistance, cSize, BLUE);
      else if (grid[x][y] == 5)
        tft.fillCircle((x + 1)*elementDistance, (y + 1)*elementDistance, cSize, PURPLE);
    }
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
          matched[x][n] = displayMatch(grid[x][n]);//matched elements are displayed as their corresponding match value
        }
      }
      else if (match > 2) {
        matched[x][y] = displayMatch(grid[x][y]);
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
          matched[n][y] = displayMatch(grid[n][y]);
        }
      }
      else if (match > 2) {
        matched[x][y] = displayMatch(grid[x][y]);
      }

      r = grid[x][y];
    }
    r = -1;
    match = 0;
  }
  return change;
}

//Deletes matches, moves elements down, and generates new elements
void updateGrid(int grid[ROWS][COLUMNS], int matched[ROWS][COLUMNS]) {
  //countPoints(matched);

  //delete matches
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      if (matched[x][y] < 0) {
        grid[x][y] = -1;
      }
    }
  }

  Serial.println("grid should be full of -1s\n");
  printGrid(grid);

  //shift down
  for (int x = ROWS - 1; x >= 0; x--) {//starting from the bottom row
    for (int y = 0; y < COLUMNS; y++) {//starting from the left-most column
      if (grid[x][y] == -1) {
        for (int a = x - 1; a >= 0; a--) {
          if (grid[a][y] != -1) {
            grid[x][y] = grid[a][y];
            grid[a][y] = -1;
            break;
          }
        }
      }
    }
  }

  Serial.println("after shifting down\n");
  printGrid(grid);

  //refill
  for (int x = 0; x < ROWS; x++) {
    for (int y = 0; y < COLUMNS; y++) {
      if (grid[x][y] == -1) {
        grid[x][y] = rand() % 6;
      }
    }
  }

  Serial.println("after refilling\n");
  printGrid(grid);

  reset(matched);

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
  //switch two values in grid
  int r;

  //switch values
  r = grid[x1][y1];
  grid[x1][y1] = grid[x2][y2];
  grid[x2][y2] = r;

  Serial.println("\n");
  printGrid (grid);
}

/*********************************  SETUP  *****************************************/

void setup() {
  Serial.begin(9600);

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
  //////////////////////////////////////////////////////////////////
  tft.begin(identifier);

  //background
  tft.fillScreen(BLACK);
  delay(2000);


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
      grid[x][y] = rand() % 6;
    }
  }

  //create matched, set everything in matched to 0
  Serial.println("original matched: ");
  printGrid(matched);

  //create posMove, set everything in posMove to 0
  Serial.println("original posMove: ");
  printGrid(posMove);



  //checks original grid for matches and updates
  if (checkGrid(grid, matched)) {
    Serial.println("there are matches!");
    printGrid(matched);
    updateGrid(grid, matched);
    Serial.println("updated grid!");
  }

  //checks if match-free grid has any possible moves
  possible = movePossible(grid, posMove);
  Serial.print("is move possible? ");
  Serial.println(possible);
  printGrid(posMove);
  while (!possible) {
    Serial.println("move is not possible :(");
    shuffle(grid);

    if (checkGrid(grid, matched)) {
      Serial.println("there are matches!");
      printGrid(matched);
      updateGrid(grid, matched);
      Serial.println("updated grid");
    }
    Serial.println("after checking for matches: ");
    printGrid(grid);

    reset(posMove);
    possible = movePossible(grid, posMove);
    Serial.print("is move possible? ");
    Serial.println(possible);
    Serial.println("possible moves: ");
    printGrid(posMove);
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
    p.y = (tft.height() - map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    //    bool isTouching(void);

    //Check each grid to see if it's been pressed
    for (int i = 0; i < 5; i++) {
      for (int j = 0; j < 6; j++) {
        if (p.x > (20 + elementDistance * i) && p.x < (20 + elementDistance * (i + 1))
            && p.y > (40 + elementDistance * j) && p.y < (40 + elementDistance * (j + 1))) {
          isTouched[i][j] = true;
          if (i != x1 || j != y1) {
            selectElement++;
          }

          if (selectElement == 1) {
            x1 = i;
            y1 = j;
            tft.drawRect(tft.width() - (20 + elementDistance * (i + 1)), tft.height() - (20 + elementDistance * (j + 2)),
                         elementDistance + 1, elementDistance + 1, RED);
            Serial.print("Grid "); Serial.print(x1); Serial.print(" "); Serial.println(y1);  //Print grid

          } else if (selectElement == 2) {
            x2 = i;
            y2 = j;

            tft.drawRect(tft.width() - (20 + elementDistance * (i + 1)), tft.height() - (20 + elementDistance * (j + 2)),
                         elementDistance + 1, elementDistance + 1, RED);
            Serial.print("Grid "); Serial.print(x2); Serial.print(" "); Serial.println(y2);  //Print grid

            //    selectElement++;
            //       if (selectElement <= 2) {


            if (x1 == x2 || y1 == y2) {
              switchValues(grid, x1, y1, x2, y2);
              /*if (checkGrid(grid, matched)) {

                  //print grid
                  printgrid(grid);
                  updateGrid(grid, matched);

                  if (!movePossible(grid)) shuffle;

                  moves--;
                  continue;
                } else  {
                  switchValues (grid, x1, y1, x2, y2);
                }*/

              do {
                if (checkGrid(grid, matched)) {
                  Serial.println("there are matches!");
                  printGrid(matched);
                  updateGrid(grid, matched);
                  Serial.println("updated grid");
                }
                Serial.println("after checking for matches: ");
                printGrid(grid);

                possible = movePossible(grid, posMove);
                Serial.print("is move possible? ");
                Serial.println(possible);
                Serial.println("possible moves: ");
                printGrid(posMove);
                reset(posMove);

                if (!possible) {
                  Serial.println("move is not possible :(");
                  shuffle(grid);
                }
              } while (!possible);
            }

          } else if (selectElement > 2) {
            selectElement = 0;
            tft.drawRect(tft.width() - (20 + elementDistance * (i + 1)), tft.height() - (20 + elementDistance * (j + 2)),
                 elementDistance + 1, elementDistance + 1, WHITE);
          }

          Serial.println("Error: switch not possible ");
          printGrid(grid);

        }
      }

    }


  }


  //  for (int i = 0; i < 5; i++) {
  //    for (int j = 0; j < 6; j++) {
  //      if (isTouched[i][j]) {
  //        Serial.print("Grid "); Serial.print(i); Serial.print(" "); Serial.println(j);  //Print grid
  //
  //        //Highlight the grid that's been pressed
  //
  //    }
  //  }
  //
  //  }


}
