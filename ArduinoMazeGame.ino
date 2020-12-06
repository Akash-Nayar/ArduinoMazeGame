#include <Adafruit_LSM6DS33.h>
#include <Adafruit_NeoMatrix.h>

#define PIN 6
#define BLUE 31
#define BLACK 1
#define DELAY 250
using namespace std;

#define LSM_CS 10
#define LSM_SCK 13
#define LSM_MISO 12
#define LSM_MOSI 11

//int x = 1;
//int y = 15;

int x = 14;
int y = 14;
bool win = false;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(16, 16, PIN,
                            NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB            + NEO_KHZ800);

const int board[16][16] PROGMEM = {
  {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
  {1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1},
  {1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 0, 1, 1, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1},
  {1, 0, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 1},
  {1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1},
  {1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1},
  {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1},
  {1, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0, 1},
  {1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 1},
  {1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 1},
  {1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1},
  {1, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1},
  {1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1},
  {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

Adafruit_LSM6DS33 lsm6ds33;
void setup(void) {

  Serial.begin(115200);
  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
    
  if (!lsm6ds33.begin_SPI(LSM_CS, LSM_SCK, LSM_MISO, LSM_MOSI)) {
    Serial.println(F("F"));
    while (1) {
      delay(10);
    }
  }

  Serial.println(F("W"));

  lsm6ds33.configInt1(false, false, true); // accelerometer DRDY on INT1
  lsm6ds33.configInt2(false, true, false); // gyro DRDY on INT2

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
}

int getY(int x, int y) {
  if (x % 2 == 1) {
    return y;
  }
  else {
    return 15 - y;
  }
}

void loop() {
  // get new normalized sensor event
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds33.getEvent(&accel, &gyro, &temp);
  float accel_x = accel.acceleration.x;
  float accel_y = accel.acceleration.y;

  // postive x is right, positive y is up
  // threshold is 2 radians

  int old_x = x;
  int old_y = y;

  if (accel_x > 2 | accel_x < -2) {
    if (accel_x > 0) {
      if (x > 0) {
        x--;
      }
    }
    else {
      if (x < 15) {
        x++;
      }
    }
  }
  else if (accel_y > 2 | accel_y < -2) {
    if (accel_y > 0) {
      if (y > 0) {
        y--;
      }
    }
    else {
      if (y < 15) {
        y++;
      }
    }
  }

  // check to see if board[y][x] is 0
  if (pgm_read_word(&board[y][x]) == 1) {
    y = old_y;
    x = old_x;
  }

  // check to see if it's a win
  if (x == 14 &&  getY(x, y) == 15){
    matrix.setCursor(0, 0);
    matrix.fillScreen(0);
    matrix.print(F("You win!"));
    win = true;
  }

  if ((old_x != x | old_y != y) and win != true) {
    // update board elements
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        int item = pgm_read_word(&board[j][i]);
        if (item == 1) {
          matrix.drawPixel(j, getY(j, i), BLUE);
        }
        if (item == 0) {
          matrix.drawPixel(j, getY(j, i), BLACK);
        }
      }
    }
    matrix.drawPixel(y, getY(y, x), matrix.Color(255, 255, 255));
  }

  matrix.show();
  delay(DELAY);
}
