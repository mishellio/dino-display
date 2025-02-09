/*
 * This sketch demonstrates the ability to set all channels of 
 * a TLC59116 driver simultaneously using the setPattern() function.
 */

#include <TLC59116.h>
#include "images.h"

// #define RESET_PIN D7
#define RESET_PIN 7

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

void setup() {
  Wire.begin();
  Wire.setClock(400000);

  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(5);
  digitalWrite(RESET_PIN, HIGH);
  Serial.begin(9600);

  // img_to_binary(DINO, binary);
  img_to_binary(DINO_SCENE_1, binary);
  img_to_binary(DINO_SCENE_2, binary2);
  img_to_binary(DINO_SCENE_3, binary3);
  
  board1.begin();
  board2.begin();
  board3.begin();
  Serial.println("setup() done");
}

void loop() {
  binary_to_led_all();
  // sanity_check_leds();
  delay(101);
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

void img_to_binary(const int img[][COLUMN], int bin[]) {
  for(int col = 0; col < COLUMN; col++) {
    for(int row = 0; row < ROW; row++) {
      bin[col] |= img[row][col] << row;
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