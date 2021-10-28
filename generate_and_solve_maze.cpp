#include <stdio.h>
#include <queue>
#include <iostream>
#include <cstdlib> // for random
#include <time.h>
#include <stack>
using namespace std;

#define WIDTH 16
#define HEIGHT 16


void swapVal(int *val_1, int *val_2)
{
    int temp = *val_1;
    *val_1 = *val_2;
    *val_2 = temp;
    return;
}

void flipMazeVals(int maze[WIDTH][HEIGHT])
{
    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            if(maze[i][j] == 1)
            {
                maze[i][j] = 0;
            }
            else
            {
                maze[i][j] = 1;
            }
        }
    }
    return;
}

void generateMazeItr(int maze[WIDTH][HEIGHT], int row, int col)
{
    stack<int> cellStack_row;
    stack<int> cellStack_col;
    cellStack_row.push(row);
    cellStack_col.push(col);
    stack<int> prev_cellStack_row;
    stack<int> prev_cellStack_col;
    prev_cellStack_row.push(-1);
    prev_cellStack_col.push(-1);    
    int prev_row = -1;
    int prev_col = -1;
    int randDirArr[4] = {0,1,2,3};
    while(!cellStack_row.empty() && !cellStack_col.empty())
    {
        // pop cell from stack and make it current cell
        row = cellStack_row.top();
        col = cellStack_col.top();
        cellStack_row.pop();
        cellStack_col.pop();

        prev_row = prev_cellStack_row.top();
        prev_col = prev_cellStack_col.top();
        prev_cellStack_row.pop();
        prev_cellStack_col.pop();

        // check if cell in bounds and not visited 
        if(row < 0 || row >= HEIGHT || col < 0 || col >= WIDTH || maze[row][col] >= 1)
        {
            continue;
        }

        // check if touching another marked cell (don't check the prev location it moved from)
        // check top
        if(row - 1 >= 0 && maze[row-1][col] >= 1 && row - 1 != prev_row) {continue;}
        // check bottom
        else if(row + 1 < HEIGHT && maze[row+1][col] >= 1 && row + 1 != prev_row) {continue;}
        // check left
        else if(col - 1 >= 0 && maze[row][col-1] >= 1 && col - 1 != prev_col) {continue;}
        // check right
        else if(col + 1 < WIDTH && maze[row][col+1] >= 1 && col + 1 != prev_col) {continue;}


        // mark as visited
        maze[row][col] = 1;
        
        prev_row = row;
        prev_col = col;

        // shuffle the direction array
        for(int i = 0; i < 20; i++)
        {
            swapVal(&randDirArr[rand() % 4], &randDirArr[rand() % 4]);
        }

        for(int i = 0; i < 4; i++)
        {
            if(randDirArr[i] == 0)
            {
                // go up
                cellStack_row.push(row-1);
                cellStack_col.push(col);
                prev_cellStack_row.push(row);
                prev_cellStack_col.push(col);
            }
            if(randDirArr[i] == 1)
            {
                // go down
                cellStack_row.push(row+1);
                cellStack_col.push(col);
                prev_cellStack_row.push(row);
                prev_cellStack_col.push(col);             
            }
            if(randDirArr[i] == 2)
            {
                // go left
                cellStack_row.push(row);
                cellStack_col.push(col-1);
                prev_cellStack_row.push(row);
                prev_cellStack_col.push(col);              
            }
            if(randDirArr[i] == 3)
            {
                // go right
                cellStack_row.push(row);
                cellStack_col.push(col+1);
                prev_cellStack_row.push(row);
                prev_cellStack_col.push(col);            
            }                
        }    
    }
    flipMazeVals(maze);
    return;
}

bool solveMazeItr(int maze[HEIGHT][WIDTH], int row, int col, int end_i, int end_j)
{
    stack<int> cellStack_row;
    stack<int> cellStack_col;
    cellStack_row.push(row);
    cellStack_col.push(col);
    int randDirArr[4] = {0,1,2,3};
    while(!cellStack_row.empty() && !cellStack_col.empty())
    {
        // pop cell from stack and make it current cell
        row = cellStack_row.top();
        col = cellStack_col.top();
        cellStack_row.pop();
        cellStack_col.pop();
        
        // check if cell in bounds and not visited and not a wall
        if(row < 0 || row >= HEIGHT || col < 0 || col >= WIDTH || maze[row][col] == 1 || maze[row][col] == 5)
        {
            continue;
        }

        // mark as visited
        maze[row][col] = 5;
        
        // found exit
        if(row == end_i && col == end_j)
        {
            return true;
        }

        // shuffle the direction array
        for(int i = 0; i < 20; i++)
        {
            swapVal(&randDirArr[rand() % 4], &randDirArr[rand() % 4]);
        }

        for(int i = 0; i < 4; i++)
        {
            if(randDirArr[i] == 0)
            {
                // go up
                cellStack_row.push(row-1);
                cellStack_col.push(col);
            }
            if(randDirArr[i] == 1)
            {
                // go down
                cellStack_row.push(row+1);
                cellStack_col.push(col);            
            }
            if(randDirArr[i] == 2)
            {
                // go left
                cellStack_row.push(row);
                cellStack_col.push(col-1);            
            }
            if(randDirArr[i] == 3)
            {
                // go right
                cellStack_row.push(row);
                cellStack_col.push(col+1);          
            }                
        }    
    }

    return false;
}

void dispMaze(int maze[HEIGHT][WIDTH])
{
    for(int i = 0; i < HEIGHT; i++)
    {
        for(int j = 0; j < WIDTH; j++)
        {
            cout << maze[i][j] << " ";
        }
        cout << "\n";
    }
    return;
}
int main()
{
    srand(time(NULL));
    int maze[HEIGHT][WIDTH] = {0};
    int start_row = 0;
    int start_col = 0;
    int end_row = 0;
    int end_col = 0;
    generateMazeItr(maze, rand() % HEIGHT, rand() % WIDTH);

    dispMaze(maze);   

    cout << "Enter Start Row: \n";
    cin >> start_row;
    cout << "Enter Start Col: \n";
    cin >> start_col;
    cout << "Enter End Row: \n";
    cin >> end_row;
    cout << "Enter End Col: \n";
    cin >> end_col;

    if(solveMazeItr(maze, start_row, start_col, end_row, end_col))
    {
        cout << "Soln: \n";
        dispMaze(maze);
    }
    else
    {
        cout << "No soln!\n";
    }

    return 0;
}