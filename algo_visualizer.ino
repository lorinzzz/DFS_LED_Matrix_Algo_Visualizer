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
// NOTE GRID COORD (0,0) is at top left corner
int maze[MATRIX_HEIGHT][MATRIX_WIDTH] = {
           {0,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0},
           {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
           {1,0,1,1,1,1,1,1,1,0,1,1,0,1,0,1},
           {1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1},
           {1,0,1,1,0,1,1,1,0,1,1,1,0,1,0,1},
           {1,0,1,1,0,1,1,1,0,1,1,0,0,1,0,1},
           {1,0,1,1,0,1,1,1,0,0,0,0,1,1,0,1},
           {1,0,1,1,0,1,1,1,0,1,1,0,0,0,0,1},
           {1,0,1,1,0,1,1,1,0,1,1,1,1,1,0,1},
           {1,0,1,1,0,1,1,1,0,1,1,1,1,1,0,1},
           {1,0,1,1,0,1,1,1,1,1,1,1,1,1,0,1},
           {1,0,1,1,0,1,1,1,1,1,1,1,1,1,0,1},
           {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
           {1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
           {1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1},
           {1,1,1,1,1,1,1,1,0,1,1,1,1,1,1,1}};
           
 
        
int findPath(int maze[MATRIX_HEIGHT][MATRIX_WIDTH], int curr_x, int curr_y, int end_x, int end_y)
{
    // path found 
    if(curr_x == end_x && curr_y == end_y)
    {
        maze[curr_x][curr_y] = 5;
        return 1;
    }
    // hit wall or edge
    if(curr_x < 0 || curr_x >= MATRIX_WIDTH || curr_y < 0 || curr_y >= MATRIX_HEIGHT || (maze[curr_x][curr_y] == 1) || (maze[curr_x][curr_y] == 5 || (maze[curr_x][curr_y] == 2)))
    {
        return 0;
    }
    // mark current pos as visited
    maze[curr_x][curr_y] = 5;

    // go left
    if(findPath(maze,   curr_x - 1, curr_y, end_x, end_y))
    {
        return 1;
    }
    // go right
    if(findPath(maze,   curr_x + 1, curr_y, end_x, end_y))
    {
        return 1;
    }
    // go up
    if(findPath(maze,   curr_x, curr_y + 1, end_x, end_y))
    {
        return 1;
    }
    // go down
    if(findPath(maze,  curr_x, curr_y - 1, end_x, end_y))
    {
        return 1;
    }
    maze[curr_x][curr_y] = 2;
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


void drawMatrix(int maze[MATRIX_HEIGHT][MATRIX_WIDTH])
{
  for(int i = 0; i < MATRIX_HEIGHT; i++)
  {
    for(int j = 0; j < MATRIX_WIDTH; j++)
    {
      if(maze[i][j] == 1)
      {
        matrix.drawPixel(i, j, colors[1]); 
      }
      if(maze[i][j] == 5)
      {
        matrix.drawPixel(i, j, colors[3]); 
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
  matrix.setBrightness(2);
  // for joystick
  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);

  //for UART 
  Serial.begin(9600);
  

}


int count = 0; 
void loop()
{
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  readJoyStick();
  drawMatrix(maze);

  // SW is active low
  if(!digitalRead(SW_PIN))
  {
    //Serial.print("Calling dfs\n");
    findPath(maze, 0, 0, 0, 15);
  }
  else
  {
    matrix.drawPixel(x, y, colors[0]); 
  }
  
  matrix.show();
  delay(100);
}
