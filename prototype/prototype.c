#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 5
#define COLUMNS 6

void printArray(int array[ROWS][COLUMNS]) {
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLUMNS; y++) {
            if (array[x][y] == -1) {
                printf("%d  ", array[x][y]);
            } else {
                printf(" %d  ", array[x][y]);
            }
        }
        printf("\n");
    }
}

bool checkGrid (int array[ROWS][COLUMNS], int row, int col ) { //perhaps replace parameters bc of header?
    int matched [row][col];
    int match = 0, r = -1;
    bool change = false;

    //set everything in matched to 0
    for (int x = 0; x < row; x ++) {
        for (int y = 0; y < col; y ++) {
            matched[x][y] = 0;
        }
    }
    
    //check rows for matches
    for (int x = 0; x < row; x ++) {
        for (int y = 0; y < col; y ++) {
            if (array[x][y] == r) {
                match++;
            } else {
                match = 0;
            }

            if (match == 2) { //if two matches = 3 in a row
                change = true;
                for (int n = y; n > y-3; n--) {
                    matched[x][n] = 1;//matched elements are displayed as 1
                }    
            }
            else if (match > 2) {
                matched[x][y] = 1;
            }

            r = array[x][y];
        }
        r = -1;
        match = 0;
    }

    //repeat above code for columns
    for (int y = 0; y < col; y ++) {
        for (int x = 0; x < row; x ++) {
            if (array[x][y] == r) {
                match++;
            } else {
                match = 0;
            }
 
            if (match == 2) {
                change = true;
                for (int n = x; n > x - 3; n--) {
                    matched[n][y] = 1;
                }
            }
            else if (match > 2) {
                matched[x][y] = 1;
            }

            r = array[x][y];
        }
        r = -1;
        match = 0;
    }

    printf("\nshow matches\n");
    for (int x = 0; x < row; x ++) {
        for (int y = 0; y < col; y ++) {
            printf(" %d  ", matched[x][y]);
        }
        printf("\n");
    }
    
    //delete matches
    for (int x = 0; x < row; x++) {
        for (int y = 0; y < col; y++) {
            if (matched[x][y] == 1) {
                array[x][y] = -1;
            }
        }
    }

    printf("\nafter deleting matches\n");
    printArray(array);

    return change;
}

void updateGrid(int array[ROWS][COLUMNS], int x, int y) { 
    //shift down
    for (int x = ROWS - 1; x >= 0; x--) {//starting from the bottom row
        for (int y = 0; y < COLUMNS; y++) {//starting from the left-most column
            if (array[x][y] == -1) {  
                for (int a = x-1; a >= 0; a--) {
                    if (array[a][y] != -1) {
                        array[x][y] = array[a][y];
                        array[a][y] = -1;
                        break;
                    }
                }
            }
        }
    }

    //print after shifting down
    printf("\nafter shifting down\n");
    printArray(array);

    //refill
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLUMNS; y++) {
            if (array[x][y] == -1) {
                array[x][y] = rand() % 6;
            }
        }
    }

    //print array after refill
    printf("\nafter refill\n");
    printArray(array);

    //check new array for matches
    while(checkGrid(array, ROWS, COLUMNS)) {
        updateGrid(array, ROWS, COLUMNS);
    }
}


void shuffle(int array[ROWS][COLUMNS]) {
    printf("\nafter shuffling\n");
    for (int x = 0; x < ROWS; x ++) {
        for (int y = 0; y < COLUMNS; y++){
            array[x][y] = rand()%6;
            printf(" %d  ", array[x][y]);
        }
        printf("\n");
    }
}

bool movePossible(int array[ROWS][COLUMNS]) {
    int a = 0;
    bool isPossible = false;

    for (int x = 0; x < ROWS; x++) {//checks from left to right
        for (int y = 0; y < COLUMNS; y++) {//checks from top to bottom
            a = array[x][y];
            
            //check rows
            if (y+1 < COLUMNS && array[x][y+1] == a) {
                if (y-2 >= 0 && array[x][y-2] == a) {
                    isPossible = true;
                    array[x][y-2] = 6;//elements that could be matched are indicated with 6
                } else if (y-1 >= 0) {
                    if (x-1 >= 0 && array[x-1][y-1] == a) {
                        isPossible = true;
                        array[x-1][y-1] = 6;
                    } else if (x+1 < ROWS && array[x+1][y-1] == a) {
                        isPossible = true;
                        array[x+1][y-1] = 6;
                    }
                } else if (y+2 < COLUMNS) {
                    if (x-1 >= 0 && array[x-1][y+2] == a) {
                        isPossible = true;
                        array[x-1][y+2] = 6;
                    } else if (x+1 < ROWS && array[x+1][y+2] == a) {
                        isPossible = true;
                        array[x+1][y+2] = 6;
                    }
                } else if (y+3 < COLUMNS && array[x][y+3] == a) {
                    isPossible = true;
                    array[x][y+3] = 6;
                }
            }

            if(isPossible) {
                array[x][y] = 6;
                array[x][y+1] = 6;
                return isPossible;
            }

            //check columns
            if (x+1 < ROWS && array[x+1][y] == a) {
                if (x-2 >= 0 && array[x-2][y] == a) {
                    isPossible = true;
                    array[x-2][y] = 6;
                } else if (x-1 >= 0) {
                    if (y-1 >= 0 && array[x-1][y-1] == a) {
                        isPossible = true;
                        array[x-1][y-1] = 6;
                    } else if (y+1 < COLUMNS && array[x-1][y+1] == a) {
                        isPossible = true;
                        array[x-1][y+1] = 6;
                    }
                } else if (x+2 < ROWS) {
                    if (y-1 >= 0 && array[x+2][y-1] == a) {
                        isPossible = true;
                        array[x+1][y-1] = 6;
                    } else if (y+1 < COLUMNS && array[x+2][y+1] == a) {
                        isPossible = true;
                        array[x+2][y+1] = 6;
                    }
                } else if (x+3 < ROWS && array[x+3][y] == a) {
                    isPossible = true;
                    array[x+3][y] = 6;
                }
            }

            if(isPossible) {
                array[x][y] = 6;
                array[x+1][y] = 6;
                return isPossible;
            }
        }
    }

    while(!isPossible) {
        printf("^no possible move\n");
        shuffle(array);
        isPossible = movePossible(array);
    }

    return isPossible;//if none of the previous conditions were satisfied, no move is possible
}

//void countPoints(int array[ROWS][COLUMNS, int x, int y]); //count points

int main (void) {
    int grid[ROWS][COLUMNS];
    srand (time(NULL));

    printf("print array\n");
    for (int x = 0; x < ROWS; x ++) {
        for (int y = 0; y < COLUMNS; y++){
            grid[x][y] = rand()%6;
            printf(" %d  ", grid[x][y]);
        }
        printf("\n");
    }
    
    if (checkGrid(grid, ROWS, COLUMNS)) {
        updateGrid(grid, ROWS, COLUMNS);
    }

    bool possibleMove = movePossible(grid);
    
    printf("\nafter scanning for possible moves\n");
    printArray(grid);

    if(possibleMove) {
        printf("Move is possible!\n");
    }

    return 0;
}

