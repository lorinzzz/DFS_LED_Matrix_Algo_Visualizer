#include <ArduinoQueue.h>
#include <gamma.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

// Define Arduino pin numbers
#define LED_DATA_PIN 6 // data pin for LED Matrix
#define SW_PIN 2 // digital pin connected to switch output
#define X_PIN A0 // analog pin connected to X output
#define Y_PIN A1 // analog pin connected to Y output


// define LED matrix width and height
#define MATRIX_HEIGHT 16
#define MATRIX_WIDTH 16

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_GRBW    Pixels are wired for GRBW bitstream (RGB+W NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(MATRIX_HEIGHT, MATRIX_WIDTH, LED_DATA_PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

// colors array
const uint16_t colors[] =
{
  matrix.Color(255, 255, 255), // 0 white
  matrix.Color(255, 0, 0), // 1 red
  matrix.Color(0, 255, 0), // 2 green
  matrix.Color(0, 0, 255), // 3 blue
};
int x = 0;
int y = 0;
int x_dir = 0;
int y_dir = 0;
int randomDirArr[4] = {0,1,2,3}; // to be shuffled later

// start location and end location of path
int start_x = -1;
int start_y = -1;
int end_x = -1;
int end_y = -1;
// NOTE GRID COORD (0,0) is at top left corner
// 1 designates a wall
// 2 is reserved 
// 3- 255 traverse order markings
// neg numbers: non soln cells
int maze[MATRIX_HEIGHT][MATRIX_WIDTH] = {
           {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
           {0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,1},
           {1,0,1,1,1,1,1,1,1,0,1,1,0,1,0,1},
           {1,0,0,0,0,0,1,0,0,0,0,1,0,1,0,1},
           {1,0,1,1,0,1,1,1,0,1,1,1,0,1,0,1},
           {1,0,1,1,0,0,0,1,0,1,1,0,0,1,0,1},
           {1,0,1,1,0,1,0,0,0,0,0,0,1,1,0,1},
           {1,0,1,1,1,0,0,1,0,1,1,0,0,0,0,1},
           {1,0,0,0,1,1,1,1,0,1,1,0,1,1,0,1},
           {1,0,1,0,0,0,0,0,0,1,0,0,1,1,0,1},
           {1,0,1,1,0,1,1,0,1,1,0,1,1,0,0,0},
           {1,0,1,1,0,1,1,0,1,1,0,1,1,1,0,1},
           {1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0},
           {1,0,1,0,1,0,1,1,0,1,0,1,0,1,1,0},
           {0,0,0,0,1,0,1,1,0,0,0,0,0,0,1,0},
           {0,1,1,1,1,0,1,1,0,1,0,1,1,1,1,0}};
           
// in efforts to reduce memory usage after all recursive call stacks of findPath is executed
// maze will be updated with the traversal order marked on the cells (barring all walls and already traversed cells)
// all traversed cells that lead to no soln (after all 4 directions return false) will be marked as negative
// thus when the matrix is read back later, we can distinguish the order of which nodes were travelled and can be traced back
// so after tracing, we can show the solns by simply showing all cells with value that are pos (>=3 to avoid walls)       
int findPath(int maze[MATRIX_HEIGHT][MATRIX_WIDTH], int curr_x, int curr_y, int end_x, int end_y, int *count)
{
    // path found 
    if(curr_x == end_x && curr_y == end_y)
    {
      Serial.print("path found!\n");
        maze[curr_x][curr_y] = 5;
        return 1;
    }


    // hit wall or edge
    if(curr_x < 0 || curr_x >= MATRIX_WIDTH || curr_y < 0 || curr_y >= MATRIX_HEIGHT || (maze[curr_x][curr_y] >= 1))
    {
        return 0;
    }    

    // mark current pos as visited
    (*count)++;
    maze[curr_x][curr_y] = *count;
    

    for(int i = 0; i < 4; i++)
    {
      // go left
      if(randomDirArr[i] == 0)
      {
        if(findPath(maze,   curr_x - 1, curr_y, end_x, end_y, count))
        {
            return 1;
        }        
      }
      // go right
      if(randomDirArr[i] == 1)
      {     
        if(findPath(maze,   curr_x + 1, curr_y, end_x, end_y, count))
        {
            return 1;
        }
      }
      // go up
      if(randomDirArr[i] == 2)
      {
        if(findPath(maze,   curr_x, curr_y + 1, end_x, end_y, count))
        {
            return 1;
        }
      }
      // go down
      if(randomDirArr[i] == 3)
      {
        if(findPath(maze,  curr_x, curr_y - 1, end_x, end_y, count))
        {
            return 1;
        }
      }      
    }
    // mark all cells that lead to no soln as negative
    maze[curr_x][curr_y] = maze[curr_x][curr_y] * (-1);
    return 0;

}


// assuming matrix width and height are not zero-indexed
// check if the movement will bring it out of bounds, if so return false
// x_pos, y_pos are the current pos, x_move and y_move is is move is made, 0 for no movement, 1 for +dir, -1 for -dir
bool isValidMovement(int x_pos, int y_pos, int x_move, int y_move, bool joyStick)
{
  // check if out of boundaries on all sides
  if(x_pos + x_move >= MATRIX_WIDTH || x_pos + x_move < 0 ||
     y_pos + y_move >= MATRIX_HEIGHT || y_pos + y_move < 0)
  {
    return false;
  }

  for(int i = 0; i < sizeof(maze)/sizeof(maze[0]); i++)
  {
    for(int j = 0; j < sizeof(maze[0])/sizeof(maze[0][0]); j++)
    {
      if(maze[i][j] == 1 || maze[i][j] == 5)
      {
        // j & i swapped since joystick has its x and y swapped
        if(x_pos + x_move == i && y_pos + y_move == j && joyStick)
        {
          return false;
        }
        if(x_pos + x_move == j && y_pos + y_move == i && !joyStick)
        {
          return false;
        }
      }
    }
  }
  return true;
}


void drawMatrix(int maze[MATRIX_HEIGHT][MATRIX_WIDTH], int state, int *itr)
{

  for(int i = 0; i < MATRIX_HEIGHT; i++)
  {
    for(int j = 0; j < MATRIX_WIDTH; j++)
    {
      // always draw walls
      if(maze[i][j] == 1)
      {
        matrix.drawPixel(i, j, colors[1]); 
      }
      // trace the traversal - state 3
      // draw only the cells >= 3 and <= *itr as *itr is incremented in the main loop as well as the walls
      if( (abs(maze[i][j]) <= *itr && abs(maze[i][j]) >= 3 == 1) && state == 3)
      {
        matrix.drawPixel(i, j, colors[3]); 
        // draws the leading cell green as it traverses
        if(abs(maze[i][j]) == *itr)
        {
          matrix.drawPixel(i, j, colors[2]); 
        }
      } 
      //  state 4: draw the soln for cells [3,*itr]
      // avoid drawing all cells that are negative
      if(maze[i][j] >= 3  && state == 4) 
      {
        matrix.drawPixel(i, j, colors[3]);     
      }
      if(state == 5)
      {
        matrix.drawPixel(start_x, start_y, colors[3]);  
        matrix.drawPixel(end_x, end_y, colors[3]);  
      }
    }
  }  

}

void readJoyStick()
{
  if(analogRead(X_PIN) >= 650)
  {
    if(isValidMovement(x, y, 1, 0, 1))
    {
      x++;
    }
  }
  else if(analogRead(X_PIN) <= 370)
  {
    if(isValidMovement(x, y, -1, 0, 1))
    {
      x--;
    }
  }
  if(analogRead(Y_PIN) <= 400)
  {
    if(isValidMovement(x, y, 0, 1, 1))
    {
      y++;
    }
  } 
  else if(analogRead(Y_PIN) >= 680)
  {
    if(isValidMovement(x, y, 0, -1, 1))
    {
      y--;
    }
  }   
}

void setup()
{
  // for LED matrix
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(5);
  // for joystick
  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);

  //for UART 
  Serial.begin(9600);

  int tempRandIdx1 = 0;
  int tempRandIdx2 = 0;
  int tempArrVal = 0;

  // seed the random num generator and shuffle the direction array
  randomSeed(analogRead(0));
  for(int i = 0; i < 20; i++)
  {
      tempRandIdx1 = random(4);
      tempRandIdx2 = random(4);
      tempArrVal = randomDirArr[tempRandIdx1];
      randomDirArr[tempRandIdx1] = randomDirArr[tempRandIdx2];
      randomDirArr[tempRandIdx2] = tempArrVal;
          
  }
}
int count = 3;
// to hold FSM state
// state = 0, draw maze, await user input start location
// state 1: await user input end location
// state 2: run algo
// state 3: trace route
// state 4: show soln
// state 5 : no soln

int state = 0;
int itr = 3;
void loop()
{
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  readJoyStick();
  // state 0: draw maze
  if(state == 0)
  {
      drawMatrix(maze, state, &itr);
      // select starting location
      if(!digitalRead(SW_PIN))
      {
        start_x = x;
        start_y = y;
        state = 1; // move to state 1
      }
  }
  else if(state == 1)
  {
      drawMatrix(maze, state, &itr);
      // show green for cursor during this stage
      matrix.drawPixel(x, y, colors[2]);
      if(!digitalRead(SW_PIN))
      {
        end_x = x;
        end_y = y;  
        state = 2; // move to state 2
      }
  }
  else if(state == 2) // run algo
  {
     if(!findPath(maze, start_x, start_y, end_x, end_y, &count))// might have x and y mixed up
     {
      state = 5;
     }
     maze[end_x][end_y] = count + 1;  
     for(int i = 0; i < 4; i++)
     {
        Serial.print(randomDirArr[i]);
        Serial.print("\n");
     }
     state = 3; // move to state 3 
  }
  else if(state == 3) // state 3: trace traversal
  {
    drawMatrix(maze, state, &itr);
    itr++;
    if(itr >= count + 1)
    {
      state = 4; // go to state 4 to show soln
    }
    delay(50); 
  }
  else if(state == 4)
  {
    drawMatrix(maze, state, &itr);
  }
  else if(state == 5)
  {
    drawMatrix(maze, state, &itr);
  }

  // draw cursor at all times except for state 1
  if(state != 1)
  {
      matrix.drawPixel(x, y, colors[0]);
  }
  
  matrix.show();
  delay(100);
}
