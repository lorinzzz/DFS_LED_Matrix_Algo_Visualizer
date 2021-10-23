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
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);

// colors array
const uint16_t colors[] =
{
  matrix.Color(255, 255, 255), // 0 white
  matrix.Color(0, 255, 0), // 1 red
  matrix.Color(0, 255, 0), // 2 green
  matrix.Color(0, 0, 255), // 3 blue
};

// assuming matrix width and height are not zero-indexed
// check if the movement will bring it out of bounds, if so return false
// x_pos, y_pos are the current pos, x_move and y_move is is move is made, 0 for no movement, 1 for +dir, -1 for -dir
bool inBoundary(int x_pos, int y_pos, int x_move, int y_move)
{
  // no movement
  if(x_move == 0 && y_move == 0)
  {
    return true;
  }

  // check if out of boundaries on all sides
  if(x_pos + x_move >= MATRIX_WIDTH || x_pos + x_move < 0 ||
     y_pos + y_move >= MATRIX_HEIGHT || y_pos + y_move < 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}



void setup()
{
  // for LED matrix
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(20);
  matrix.setTextColor(colors[0]);
  // for joystick
  pinMode(SW_PIN, INPUT);
  digitalWrite(SW_PIN, HIGH);
  Serial.begin(9600);
}

int x = 0;
int y = 0;
int x_dir = 0;
int y_dir = 0;

void loop()
{
  
  matrix.fillScreen(0);
  matrix.setCursor(0, 0);
  if(analogRead(X_PIN) >= 610)
  {
    if(inBoundary(x, y, 1, 0))
    {
      x++;
    }
  }
  else if(analogRead(X_PIN) <= 410)
  {
    if(inBoundary(x, y, -1, 0))
    {
      x--;
    }
  }
  if(analogRead(Y_PIN) >= 640)
  {
    if(inBoundary(x, y, 0, 1))
    {
      y++;
    }
  } 
  else if(analogRead(Y_PIN) <= 440)
  {
    if(inBoundary(x, y, 0, -1))
    {
      y--;
    }
  } 

  // SW is active low
  if!(digitalRead(SW_PIN))
  {
    ;
  }
  else
  {
    matrix.drawPixel(x, y, colors[0]); 
  }
  matrix.show();


  // these are for the serial monitor
  // use the analogRead and digitalRead for later 
  // when the analogs are at rest, X = 510, y = 540
  // 3 sensitivity move settings 
  // 1) Slow: -X: 310 - 410    , +X: 610 - 710     , -Y: 340 - 440    , +Y: 640 - 740
  // 2) Medium: -X: 210 - 309    , +X: 711 - 810    , -Y: 240 - 339    , +Y: 741 - 840
  // 3) Fast -X: <= 209    , +X: >= 811     , -Y: <= 239    , +Y: >= 841
  Serial.print("Switch:  ");
  Serial.print(digitalRead(SW_PIN));
  Serial.print("\n");
  Serial.print("X-axis: ");
  Serial.print(analogRead(X_PIN));
  Serial.print("\n");
  Serial.print("Y-axis: ");
  Serial.println(analogRead(Y_PIN));
  Serial.print("\n\n");
  delay(100);

}
