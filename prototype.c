#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 5
#define COLUMNS 6

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
            printf("%d  ", matched[x][y]);
        }
        printf("\n");
    }
    
    //delete matches
    for (int x = 0; x < row; x++) {
        for (int y = 0; y < col; y++) {
            if (matched[x][y] == 1) {
                array[x][y] = 0;
            }
        }
    }

    printf("\nafter deleting matches\n");
    for (int a = 0; a < row; a++) {
        for (int b = 0; b < col; b++) {
            printf("%d  ", array[a][b]);
        }
        printf("\n");
    }
    return change;
}

void updateGrid(int array[ROWS][COLUMNS], int x, int y) { 
    //shift down
    for (int x = ROWS - 1; x >= 0; x--) {//starting from the bottom row
        for (int y = 0; y < COLUMNS; y++) {//starting from the left-most column
            if (array[x][y] == 0) {  
                for (int a = x-1; a >= 0; a--) {
                    if (array[a][y] != 0) {
                        array[x][y] = array[a][y];
                        array[a][y] = 0;
                        break;
                    }
                }
            }
        }
    }

    //print after shifting down
    printf("\nafter shifting down\n");
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLUMNS; y++) {
            printf("%d  ", array[x][y]);
        }    
        printf("\n");
    }

    //refill
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLUMNS; y++) {
            if (array[x][y] == 0) {
                array[x][y] = rand() % 6 + 1;
            }
        }
    }

    //print array after refill
    printf("\nafter refill\n");
    for (int x = 0; x < ROWS; x++) {
        for (int y = 0; y < COLUMNS; y++) {
            printf("%d  ", array[x][y]);
        }
        printf("\n");
    }

    //check new array for matches
    while(checkGrid(array, ROWS, COLUMNS)) {
        updateGrid(array, ROWS, COLUMNS);
    }
}

/*bool movePossible(int array[ROWS][COLUMNS]) {
    for (int x = 0; x < ROWS; x ++) {
        for (int y = 0; y < COLUMNS; y ++) {
            //check rows
            if (y-1 < COLUMNS && array[x][y] == array[x][y-1]) {//there are 2 in a row
                if (y-2 >= 0 && array[x][y-2] == array[x][y]) {
                    return true;
                } else if (y-1 >= 0) {
                    if (x-1 >= 0 && array[x-1][y-1] == array[x][y]) {
                        return true;
                    } else if (x+1 < ROWS && array[x+1][y-1] == array[x][y]) {
                        return true;
                    }
                } else if (y+2 < COLUMNS) {
                    if (x-1 >= 0 && array[x-1][y+2] == array[x][y]) {
                        return true;
                    } else if (x+1 < ROWS && array[x+1][y+2] == array[x][y]) {
                        return true;
                    }
                } else if (y+3 < COLUMNS && array[x][y+3] == array[x][y]) {
                    return true;
                }
            //check columns
            } else if (x+1 <= ROWS && array[x][y] == array[x+1][y]) {//if there are 2 in a column
                if (x-2 >= 0 && array[x-2][y] == array[x][y]) {
                    return true;
                } else if (x-1 >= 0) {
                    if (y-1 >= 0 && array[x-1][y-1] == array[x][y]) {
                        return true;
                    } else if (y+1 < COLUMNS && array[x-1][y+1] == array[x][y]) {
                        return true;
                    }
                } else if (x + 2 < ROWS && )
            } 
        }
    }
    return false;//if none of the previous conditions were satisfied, no move is possible
}
*/

//void countPoints(int array[ROWS][COLUMNS, int x, int y]); //count points

int main (void) {
    int grid[ROWS][COLUMNS];
    srand (time(NULL));

    printf("print array\n");
    for (int x = 0; x < ROWS; x ++) {
        for (int y = 0; y < COLUMNS; y++){
            grid[x][y] = rand()%6 + 1;
            printf("%d  ", grid[x][y]);
        }
        printf("\n");
    }
    
    if (checkGrid(grid, ROWS, COLUMNS)) {
        updateGrid(grid, ROWS, COLUMNS);
    }

    return 0;
}

