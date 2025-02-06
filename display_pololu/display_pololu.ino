/* Simple test with TLC59116

  Test w/arduino-pin-7 first, to check your LED

  Then, on each TLC59116, 
  test channel 0-3 for blink
    The blink rate is different for each TLC (longer as the address increases)
    play with the trimpot for brightness
  then channel 4-7 for a triangle ramp for brightness
  then channel 8-11 for brightness

  These are at Arduino voltage, usually 3v:
  So, typical LED of 20ma means resistor of 125ohms
  Arduino Pin 7 : short blink for "safe" test of LED
    as SINK, LOW is on, and LED- to the arduino pin
  Arduino Pin 13 : Follows 7 (on board LED) (source, so HIGH is on)
    So, 7 should blink along with 13
  Arduino Pin 6 is doing the triangle ramp, but it's HIGH on, so SOURCE not SINK

  These are the TLC59116:
  At BAT=5v, a LED of 20ma means resistor 250ohm
  Output 0,1,2,3: short blink for "safe" test of LED
    Blinks n-times, then pause.
    'n' is the cardinal position of the TLC in the tlcmanager[]
    Thus, the tlcmanager[0] device has an 'n' of 1.
    This is set per TLC (individual addressed).
  Output 4,5,6,7: triangle ramp
    Increments brightness up/down by 1 on each "loop".
    All TLC at same rate.
    This is done by broadcast, not individually.
    This reflects the Wire speed, 50khz is noticeably slower than 500khz.
  Output 8,9,10,11: On
  Output 12,13,14,15: Off

  You can slow down the I2C speed (100khz default), look for the
    TLC59116Manager tlcmanager;
  below. Add arguments for speed setting, e.g.
    TLC59116Manager tlcmanager(Wire, 50000); // you have to say 'Wire', and then 50khz
  Check the console for the actual speed (only certain values possible).
*/

#include <Wire.h>
#include <avr/pgmspace.h>

// #include "Adafruit_VL53L0X.h"
// Adafruit_VL53L0X lox = Adafruit_VL53L0X();
#include <VL53L0X.h>
VL53L0X sensor;

#include "TLC59116_old.h"
TLC59116Manager tlcmanager(Wire, 100000); // see the I2C_Speed.xls spread sheet for workable speeds

const int COLUMN = 8; // num columns in 2D array
const int ROW = 8;

const int DINO[ROW][COLUMN] = {
  {0, 0, 0, 0, 1, 1, 1, 1},
  {0, 0, 0, 0, 1, 0, 1, 1},
  {1, 0, 0, 0, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 1, 1},
  {1, 1, 1, 1, 1, 1, 0, 0},
  {0, 1, 1, 1, 1, 1, 1, 0},
  {0, 1, 1, 1, 1, 0, 0, 0},
  {0, 1, 0, 1, 0, 0, 0, 0}
};

  bool detected = false;
  // long prev_time = 0;
  // long curr_time = 0;
  // long avg_rtt = 0;
  long start_time = 0;
  long s = 0;

void setup() {
    Serial.begin(9600);
    while (! Serial) {
      delay(1);
    }
    Wire.begin();

    pinMode(7,OUTPUT);
    pinMode(6,OUTPUT);
    digitalWrite(7,HIGH);
    pinMode(13,OUTPUT);
    Serial.println("setup().arduino done");

    sensor.setTimeout(500);
    if (!sensor.init())
    {
      Serial.println("Failed to detect and initialize sensor!");
      while (1) {}
    }

    sensor.setSignalRateLimit(0.4);
    sensor.setMeasurementTimingBudget(20000);
    sensor.startContinuous();

    tlcmanager.init();
    // tlcmanager.broadcast().on_pattern(0xF << 8); // 8..11 on
    // tlcmanager[0].set_milliamps(0, 150);
    Serial.println("setup().tlc done");

    Serial.println("setup() done");
}

void loop() {
  long period = get_half_period();
  Serial.print("final: ");
  Serial.println(period);
  for (int i = 0; i < 100; i++) {
      // s = micros();
      // array_to_leds(DINO);
      // long e = micros()-s;
      // Serial.print("DINO: ");
      // Serial.println(e);
      // delay(period);
      array_to_leds(DINO);
      delay(period - 13);  //13 is profiled time dino takes to flash
  }
  
  // array_to_leds_delay(DINO, period);
  // array_to_leds(DINO);
  // update_led();
  }

// sample period for 50 rotations, return average
long get_half_period() {
  long period = 0;
  int num_rotation = 0;
  while (num_rotation < 50) {
      if (sensor.readRangeContinuousMillimeters() > 2000) {
        detected = false;
      }
      else if (!detected) {
        long end_time = millis()-start_time;
        Serial.println(end_time);
        if (end_time < 170) {
          period += end_time;
          num_rotation += 1;
        }
        detected = true;
        start_time = millis();
      }
      if (sensor.timeoutOccurred()) { Serial.print(" GET FREQUENCY TIMEOUT"); }
  }
  return period/num_rotation;
}

long get_half_period_no_detect() {
  long period = 0;
  int num_rotation = 0;
  while (num_rotation < 50) {
      if (sensor.readRangeContinuousMillimeters() < 2000) {
        long end_time = millis()-start_time;
        Serial.println(end_time);
        // if (end_time < 170000) {
          period += millis()-start_time;
          num_rotation += 1;
        // }
        detected = true;
        start_time = millis();
      }
      if (sensor.timeoutOccurred()) { Serial.print(" GET FREQUENCY TIMEOUT"); }
  }
  return period/num_rotation;
}

void update_led() {
  if (sensor.readRangeContinuousMillimeters() > 2000) {
    detected = false;
  }
  else if (!detected) {
    detected = true;
    array_to_leds(DINO);
  }
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
}

// void profile_sensor_without_detect() {
//   if (sensor.readRangeContinuousMillimeters() < 2000) {
//     Serial.print("half rotate: ");
//     Serial.println(micros() - start_time);
//     detected = true;
//     array_to_leds(DINO);
//     start_time = micros();
//     // array_to_leds_delay(DINO, 1);
//   }
// }

// bool arm_distance(VL53L0X_RangingMeasurementData_t measure) {
//   return (measure.RangeMilliMeter >= 110 && measure.RangeMilliMeter <= 140); // 110 to 130 based on testing --> was a bit inaccurate so maybe 90 - 140?
//   // or maybe i just do either number or out of range, depends on if there's anything else that can interfere
// }

void array_to_leds_delay(int array[][COLUMN], long delay_time) {
  for(int half_rotation = 0; half_rotation < 50; half_rotation++) {
    unsigned int binary = 0;
    for(int i = 0; i < COLUMN; i++) {
      binary = col_to_bin(array, i);
      tlcmanager.broadcast().off_pattern(0xFFFF);
      tlcmanager.broadcast().on_pattern(binary);
    }
    tlcmanager.broadcast().off_pattern(0xFFFF);
    delayMicroseconds(delay_time);
  }
}

void array_to_leds(int array[][COLUMN]) {
  unsigned int binary = 0;
  // long start_time = micros();
  for(int i = 0; i < COLUMN; i++) {
    binary = col_to_bin(array, i);
    tlcmanager.broadcast().off_pattern(0xFFFF);
    tlcmanager.broadcast().on_pattern(binary);
    // delay(100);
  }
  tlcmanager.broadcast().off_pattern(0xFFFF);
  delay(5);
  // long total_time = micros() - start_time;
  // Serial.println(total_time);
}

unsigned int col_to_bin(int array[][COLUMN], int col_num) {
  unsigned int binary = 0;
  for(int i = 0; i < ROW; i++) {
    binary |= array[i][col_num] << i;
  }
  return binary;
}
