/*
 * This sketch demonstrates the ability to set all channels of 
 * a TLC59116 driver simultaneously using the setPattern() function.
 */

#include <TLC59116.h>
#include "images.h"

#include <VL53L0X.h>
VL53L0X sensor;

// #define RESET_PIN D7
#define RESET_PIN 7
#define IR_PIN 8

TLC59116 board1(0b1100000, true);
TLC59116 board2(0b1100001, true);
TLC59116 board3(0b1100010, true);
// TLC59116 board4(0b1100011, true);
// TLC59116 board5(0b1100100, true);
// TLC59116 board6(0b1100101, true);

// 2nd board as 1, 2, 3
// TLC59116 board1(0b1100011, true);
// TLC59116 board2(0b1100100, true);
// TLC59116 board3(0b1100101, true);

// dino frame constants
const int COLUMN = 60;
const int ROW = 16;

// singular dino constants
// const int COLUMN = 8;
// const int ROW = 8;

int binary[COLUMN] = {0};
int binary2[COLUMN] = {0};
int binary3[COLUMN] = {0};

bool detected = false;
long start_time = 0;
long s = 0;

void setup() {
  Wire.begin();
  Wire.setClock(400000);

  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(5);
  digitalWrite(RESET_PIN, HIGH);
  Serial.begin(9600);

  // sensor.setTimeout(500);
  // if (!sensor.init())
  // {
  //   Serial.println("Failed to detect and initialize sensor!");
  // }

  // sensor.setSignalRateLimit(0.4);
  // sensor.setMeasurementTimingBudget(20000);
  // sensor.startContinuous();

  // img_to_binary(DINO, binary);
  img_to_binary(DINO_SCENE, binary, 0, 16);
  img_to_binary(DINO_SCENE, binary2, 16, 32);
  img_to_binary(DINO_SCENE, binary3, 32, 48);
  
  board1.begin();
  board2.begin();
  board3.begin();
  Serial.println("setup() done");
}

void loop() {
  if (digitalRead(IR_PIN) == LOW) {
    delay(50);
    binary_to_led_all();
  }
    
  //delay(100);
  // double period = get_half_period();
  //   period -= 54500;//13 ms is profiled time dino takes to flash
  //               // 54.5 ms is profiled time for dino frame with shadow registers
  // period = period/12;
  // Serial.print("final: ");
  // Serial.println(period);

  // for (int i = 0; i < 100; i++) {
  //     // long st = micros();
  //     binary_to_led_all();
  //     // long et = micros() - st;
  //     // Serial.print("image display time: ");
  //     // Serial.println(et);
  //     for (int i = 0; i < 12; i++) {
  //       delayMicroseconds(period);  
  //     }
  // }
  // sanity_check_leds();
  // delay(101);
}

// sample period for 50 rotations, return average
long get_half_period() {
  long period = 0;
  int num_rotation = 0;
  while (num_rotation < 25) {
      if (sensor.readRangeContinuousMillimeters() > 2000) {
        detected = false;
      }
      else if (!detected) {
        long end_time = micros()-start_time;
        Serial.println(end_time);
        if (end_time < 170000) {
          period += end_time;
          num_rotation += 1;
        }
        detected = true;
        start_time = micros();
      }
      if (sensor.timeoutOccurred()) { Serial.print(" GET FREQUENCY TIMEOUT"); }
  }
  return period/num_rotation;
}

void binary_to_led(TLC59116 board) {
  // long start_time = micros();
  for(int col = 0; col < COLUMN; col++) {
    board.setPattern(binary[col], 255);
    // delay(1000);
  }
  // long end_time = micros();
  // long total_time = end_time - start_time;
  // Serial.println(total_time);
}

void binary_to_led_all() {
  for(int col = 0; col < COLUMN; col++) {
    board1.setPattern(binary[col], 255);
    board2.setPattern(binary2[col], 255);
    board3.setPattern(binary3[col], 255);
  }
  board1.setPattern(0, 255);
  board2.setPattern(0, 255);
  board3.setPattern(0, 255);
}

void img_to_binary(const int img[][COLUMN], int bin[], int row_start, int row_end) {
  for(int col = 0; col < COLUMN; col++) {
    for(int row = row_start; row < row_end; row++) {
      bin[col] |= img[row][col] << row%16;
    }
  }
}

// lights up all leds
void sanity_check_leds() {
  board1.setPattern(0xFFFF, 255);
  board2.setPattern(0xFFFF, 255);
  board3.setPattern(0xFFFF, 255);
  // board4.setPattern(0xFFFF, 255);
  // board5.setPattern(0xFFFF, 255);
  // board6.setPattern(0xFFFF, 255);
}

// checks all elements in an array
void sanity_check_array(int array[]) {
  for (int i = 0; i < COLUMN; i++) {
    Serial.print("sanity check: ");
    Serial.println(array[i]);
  }
}