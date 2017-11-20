#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define ROWS 5
#define COLUMNS 6

int points = 0;
void countPoints(int grid[ROWS][COLUMNS]);

void reset(int grid[ROWS][COLUMNS]) {//reset every element of grid to 0
    printf("resetting\n");
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            grid[i][j] = 0;
        }
    }
}

void printGrid(int grid[ROWS][COLUMNS]) {
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLUMNS; y++) {
            if (grid[x][y] < 0) {
                printf("%d  ", grid[x][y]);
            } else {
                printf(" %d  ", grid[x][y]);
            }
        }
        printf("\n");
    }
}

int displayMatch(int matchedElement) {
    switch(matchedElement) {
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
                for (int n = y; n > y-3; n--) {
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

    printf("grid should be full of -1s\n");
    printGrid(grid);

    //shift down
    for (int x = ROWS - 1; x >= 0; x--) {//starting from the bottom row
        for (int y = 0; y < COLUMNS; y++) {//starting from the left-most column
            if (grid[x][y] == -1) {  
                for (int a = x-1; a >= 0; a--) {
                    if (grid[a][y] != -1) {
                        grid[x][y] = grid[a][y];
                        grid[a][y] = -1;
                        break;
                    }
                }
            }
        }
    }

    printf("after shifting down\n");
    printGrid(grid);

    //refill
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLUMNS; y++) {
            if (grid[x][y] == -1) {
                grid[x][y] = rand() % 6;
            }
        }
    }

    printf("after refilling\n");
    printGrid(grid);

    reset(matched);

    //check new grid for matches
    if(checkGrid(grid, matched)) {
        updateGrid(grid, matched);
    }
}

void shuffle(int grid[ROWS][COLUMNS]) {
    printf("\nNo possible moves: shuffling\n");
    for (int x = 0; x < ROWS; x ++) {
        for (int y = 0; y < COLUMNS; y++){
            grid[x][y] = rand()%6;
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
            if (y+1 < COLUMNS && grid[x][y+1] == a) {
                if (y-2 >= 0 && grid[x][y-2] == a) {
                    isPossible = true;
                    posMove[x][y-2] = 6;//elements that could be matched are indicated with 6
                } else if (y-1 >= 0 && x-1 >= 0 && grid[x-1][y-1] == a) {
                        isPossible = true;
                        posMove[x-1][y-1] = 6;
                } else if (y-1 >= 0 && x+1 < ROWS && grid[x+1][y-1] == a) {
                        isPossible = true;
                        posMove[x+1][y-1] = 6;
                } else if (y+2 < COLUMNS && x-1 >= 0 && grid[x-1][y+2] == a) {
                        isPossible = true;
                        posMove[x-1][y+2] = 6;
                } else if (y+2 < COLUMNS && x+1 < ROWS && grid[x+1][y+2] == a) {
                        isPossible = true;
                        posMove[x+1][y+2] = 6;
                } else if (y+3 < COLUMNS && grid[x][y+3] == a) {
                    isPossible = true;
                    posMove[x][y+3] = 6;
                }
            }

            if(isPossible) {
                posMove[x][y] = 6;
                posMove[x][y+1] = 6;
                return isPossible;
            }

            //check columns
            if (x+1 < ROWS && grid[x+1][y] == a) {
                if (x-2 >= 0 && grid[x-2][y] == a) {
                    isPossible = true;
                    posMove[x-2][y] = 6;
                } else if (x-1 >= 0 && y-1 >= 0 && grid[x-1][y-1] == a) {
                        isPossible = true;
                        posMove[x-1][y-1] = 6;
                } else if (x-1 >= 0 && y+1 < COLUMNS && grid[x-1][y+1] == a) {
                        isPossible = true;
                        posMove[x-1][y+1] = 6;
                } else if (x+2 < ROWS && y-1 >= 0 && grid[x+2][y-1] == a) {
                        isPossible = true;
                        posMove[x+2][y-1] = 6;
                } else if (x+2 < ROWS && y+1 < COLUMNS && grid[x+2][y+1] == a) {
                        isPossible = true;
                        posMove[x+2][y+1] = 6;
                } else if (x+3 < ROWS && grid[x+3][y] == a) {
                    isPossible = true;
                    posMove[x+3][y] = 6;
                }
            }

            if(isPossible) {
                posMove[x][y] = 6;
                posMove[x+1][y] = 6;
                return isPossible;
            }
        }
    }

    return isPossible;//if none of the previous conditions were satisfied, no move is possible
}

/*void countPoints(int grid[ROWS][COLUMNS]){ //count points
    for (int x = 0; x < ROWS; x++)
        for (int y = 0; y < COLUMNS; y ++)
            if (grid[x][y] == 1) 
                points += 10;
}*/
            
int countPoints(int matched[ROWS][COLUMNS], int totalPoints, int countMatches[6]) {
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLUMNS; y++) {
            switch(matched[x][y]) {
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
        totalPoints += countMatches[i] * 10;
    }

    return totalPoints;
}            

void switchValues (int grid[ROWS][COLUMNS], int x1, int y1, int x2, int y2) {
    //switch two values in grid
    int r;

    //switch values
    r = grid[x1][y1];
    grid[x1][y1] = grid[x2][y2];
    grid[x2][y2] = r; 

    printf("\n");
    printGrid (grid);
}

int main(void) {
    srand (time(NULL));

    //generating first grid
    
    // int grid[ROWS][COLUMNS];
    // for (int x = 0; x < ROWS; x ++) {
    //     for (int y = 0; y < COLUMNS; y++){
    //         grid[x][y] = rand()%6;
    //     }
    // }
    int grid[ROWS][COLUMNS] = {{0,1,2,3,4,5}, {5,4,3,2,1,0}, {0,0,1,4,0,0}, {5,5,3,3,5,5}, {4,4,2,2,4,4}};
    printf("original grid: \n");
    printGrid(grid);

    //create matched, set everything in matched to 0
    int matched[ROWS][COLUMNS] = {0};
    printf("original matched: \n");
    printGrid(matched);

    //create posMove, set everything in posMove to 0
    int posMove[ROWS][COLUMNS] = {0};
    printf("original posMove: \n");
    printGrid(posMove);

    int totalPoints = 0;
    int countMatches[6] = {0};
    
    //checks original grid for matches and updates
    if (checkGrid(grid, matched)) {
        printf("there are matches!\n");
        printGrid(matched);
        updateGrid(grid, matched);
        printf("updated grid!\n");
    }

    //checks if match-free grid has any possible moves
    bool possible = movePossible(grid, posMove);
    printf("is move possible? %d\n", possible);
    printGrid(posMove);
    while (!possible) {
        printf("move is not possible :(\n");
        shuffle(grid);

        if (checkGrid(grid, matched)) {
            printf("there are matches!\n");
            printGrid(matched);
            updateGrid(grid, matched);
            printf("updated grid\n");
        }
        printf("after checking for matches: \n");
        printGrid(grid);

        reset(posMove);
        possible = movePossible(grid, posMove);
        printf("is move possible? %d\n", possible);
        printf("possible moves: \n");
        printGrid(posMove);
    }

    //calling input
    int moves = 20;
    int x1 = 0;
    int x2 = 0;
    int y1 = 0;
    int y2 = 0;
    while (moves > 0) {
        printf("You have %d points and %d moves.\nPlease insert two coordinates in the form x, y \n", points, moves);
        scanf("%d,%d\n%d,%d", &x1, &y1, &x2, &y2);

        printf("current grid: \n");
        printGrid(grid);
        printf ("\n");

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
                    printf("there are matches!\n");
                    printGrid(matched);
                    updateGrid(grid, matched);
                    printf("updated grid\n");
                }
                printf("after checking for matches: \n");
                printGrid(grid);

                possible = movePossible(grid, posMove);
                printf("is move possible? %d\n", possible);
                printf("possible moves: \n");
                printGrid(posMove);
                reset(posMove);

                if (!possible) {
                    printf("move is not possible :(\n");
                    shuffle(grid);
                }
            } while (!possible);
        }

        printf("Error: switch not possible \n");
        printGrid(grid);
    }

    return 0;
}
