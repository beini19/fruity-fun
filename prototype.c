#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define ROWS 5
#define COLUMNS 6

void checkGrid (int array[ROWS][COLUMNS], int row, int col );

void checkGrid (int array[ROWS][COLUMNS], int row, int col ) { //perhaps replace parameters bc of header?
    int matched [row][col];
    int match = 0, r = -1;
    bool change = false;

    //set everything in matched to 0
    for (int x = 0; x < row; x ++)
        for (int y = 0; y < col; y ++)
            matched[x][y] = 0;
    
    //check rows for matches
    for (int x = 0; x < row; x ++) {
        for (int y = 0; y < col; y ++){
            if (array[x][y] == r)
                match++;
            else
                match = 0;

            printf("%d  %d     ",r, array[x][y]); 

            if (match == 2) { //if two matches = 3 in a row
                change = true;
                for (int n = y; n > y-3; n--)
                    matched[x][n] = 1;
                
            }
            else if (match > 2)
                matched[x][y] = 1;
                      
            r = array[x][y];
        }
        r = -1;
        match = 0;
        printf ("\n");
    }

    printf ("\n");
    //repeat above code for columns
    for (int y = 0; y < col; y ++) {
        for (int x = 0; x < row; x ++) {
            if (array[x][y] == r)
                match++;
            else
                match = 0;

            printf("%d  %d     ",r, array[x][y]); 
            if (match == 2) {
                change = true;
                for (int n = x; n > x - 3; n--)
                    matched[n][y] = 1;
            
            }
            else if (match > 2)
                matched[x][y] = 1;
                  
            r = array[x][y];
        }
        printf ("\n");
        r = -1;
        match = 0;
    }

    for (int x = 0; x < row; x ++) {
        printf("\n");
        for (int y = 0; y < col; y ++)
            printf("%d   ", matched[x][y]);
        }
    /*
    //delete matches
    for (int x = 0; x < row; x ++) 
        for (int y = 0; y < col; y ++)
            if (matched[x][y]) {
                //move everything down
                moveDown(array, x, y);
            }
    */
}

void moveDown(int array[ROWS][COLUMNS], int x, int y); //MOVE EVERYTHING DOWN - todo

//void countPoints(int array[ROWS][COLUMNS, int x, int y]); //count points

int main (void) {
    
    int grid[5][6];
    srand (time(NULL));

    for (int x = 0; x < ROWS; x ++) {
        for (int y = 0; y < COLUMNS; y++){
            grid[x][y] = rand()%6;
            printf("%d   ", grid[x][y]);
        }
        printf("\n");
    }
    checkGrid(grid, ROWS, COLUMNS);

    return 0;
}
