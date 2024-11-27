/*
 * This sketch demonstrates the ability to set all channels of 
 * a TLC59116 driver simultaneously using the setPattern() function.
 */

#include <TLC59116.h>
#include "images.h"

#define RESET_PIN D7

TLC59116 board1(0b1100011); //check which address is which
TLC59116 board2(0b1100100);
TLC59116 board3(0b1100101);

const int COLUMN = 8;
const int ROW = 8;

int binary[COLUMN] = {0};
// int binary2[COLUMN] = {0};
// int binary3[COLUMN] = {0};

void setup() {
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, LOW);
  delay(5);
  digitalWrite(RESET_PIN, HIGH);
  Serial.begin(9600);

  // binary_dino = sprite_to_binary_return(DINO);
  // sanity_check(binary_dino);
  sprite_to_binary(DINO);
  // sprite_to_binary(cursed2, binary2);
  // sprite_to_binary(cursed3, binary3);
  
  board1.begin();
  board2.begin();
  board3.begin();
  Serial.println("setup() done");
}

void loop() {
  // board1.setPattern(0xFFFF, 255);
  // board2.setPattern(0xFFFF, 255);
  // board3.setPattern(0xFFFF, 255);
  // delay(1000);
  // board1.setPattern(0x0001, 255);
  // board2.setPattern(0x0001, 255);
  // board3.setPattern(0x0001, 255);
  // delay(1000);

  // board3.setPattern(0xFFFF, 255);
  // delay(1000);
  // board3.setPattern(0xFFF, 255);
  // delay(1000);
  // board3.setPattern(0xFF, 255);
  // delay(1000);
  // board3.setPattern(0xF, 255);

  binary_to_led(board3);
  // board1.setPattern(0xFFFF, 255);
  // board2.setPattern(0xFFFF, 255);
  // board3.setPattern(0xFFFF, 255);
  // binary_to_led_all();
  // delay(100);
}

void binary_to_led(TLC59116 board) {
  long start_time = micros();
  for(int col = 0; col < COLUMN; col++) {
    board.setPattern(binary[col], 255);
    // delay(1000);
  }
  long end_time = micros();
  long total_time = end_time - start_time;
  Serial.println(total_time);
}

// void binary_to_led_all() {
//   for(int col = 0; col < COLUMN; col++) {
//     board1.setPattern(binary[col], 255);
//     board2.setPattern(binary2[col], 255);
//     board3.setPattern(binary3[col], 255);
//     // delay(1000);
//   }
// }

void sprite_to_binary(const int sprite[][COLUMN]) {
  // unsigned int binary = 0;
  // unsigned int binary_array[COLUMN];
  for(int col = 0; col < COLUMN; col++) {
    for(int row = 0; row < ROW; row++) {
      binary[col] |= sprite[row][col] << row;
    }
    // Serial.println(binary);
    // binary_array[col] = binary;
    // binary = 0;
  }
  // return binary_array;
}

// int* sprite_to_binary_return(int sprite[][COLUMN]) {
//   unsigned int binary_array[COLUMN];
//   for(int col = 0; col < COLUMN; col++) {
//     unsigned int binary = 0;
//     for(int row = 0; row < ROW; row++) {
//       binary |= sprite[row][col] << row;
//     }
//     Serial.print("stb: ");
//     Serial.println(binary);
//     binary_array[col] = binary;
//     // binary = 0;
//   }
//   return binary_array;
// }

void sanity_check(int array[]) {
  for (int i = 0; i < COLUMN; i++) {
    Serial.print("sanity check: ");
    Serial.println(array[i]);
  }
}

