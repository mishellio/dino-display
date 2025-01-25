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

#include "BlinkTracking.h"

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
  long prev_time = 0;
  long curr_time = 0;
  long avg_rtt = 0;
  long start_time = 0;

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

    BlinkTracking::init_tracking();

    Serial.println("setup() done");
}

void loop() {
  static TLC59116 &t = tlcmanager[0];
  // VL53L0X_RangingMeasurementData_t measure;
  // bool full_rotation = true;

  // // do_blinks(t);
  // // do_triangles(t);
  // lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  // if (measure.RangeStatus != 4) {  // phase failures have incorrect data
  //   Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  // } else {
  //   Serial.println(" out of range ");
  // }

  // // ig see what the range is for the sensor
  // if (arm_distance(measure)) {
  //   full_rotation = !full_rotation; // need to actually check this properly
  //   // or consider just doing half for now
  // }

  // if (full_rotation) {
  //   array_to_leds(DINO);
  // }

  // array_to_leds(DINO);

  //test_sensor();
  update_led();

  }

void update_led() {

  // VL53L0X_RangingMeasurementData_t measure;
  
  // lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout! this is where measure gets set
  // Serial.println(measure.RangeMilliMeter);
  // if (arm_distance(measure) && !detected) {
  //   prev_time = curr_time; // set previous time with previous current
  //   curr_time = micros(); // new current
  //   long total_time = curr_time - prev_time;
  //   Serial.println(total_time);
  //   // Serial.println(measure.RangeMilliMeter);

  //   detected = true;
  //   array_to_leds_delay(DINO, total_time);
  // }
  // else if (!arm_distance(measure) && detected) {
  //   detected = false;
  // }
  // if (measure.RangeMilliMeter > 2000) {
  //   detected = false;
  // } else if (!detected) {
  //   detected = true;
    // if (sensor.readRangeContinuousMillimeters() < 2000) {
    //   array_to_leds_delay(DINO, 1);
    // }
    
    if (sensor.readRangeContinuousMillimeters() > 2000) {
      detected = false;
      
    }
    else if (!detected) {
      // Serial.print("half rotate: ");
      // Serial.println(micros() - start_time);
      detected = true;
      array_to_leds(DINO);
      // start_time = micros();
      // array_to_leds_delay(DINO, 1);
    }
    //  if (sensor.readRangeContinuousMillimeters() < 2000) {
    //   Serial.print("half rotate: ");
    //   Serial.println(micros() - start_time);
    //   detected = true;
    //   array_to_leds(DINO);
    //   start_time = micros();
    //   // array_to_leds_delay(DINO, 1);
    // }
    if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }
                  // Serial.println(sensor.readRangeContinuousMillimeters());


    // prev_time = curr_time; // set previous time with previous current
    // curr_time = micros(); // new current
    // long total_time = curr_time - prev_time;
    // // if (avg_rtt == 0) {
    //   // avg_rtt = total_time;
    // // } else {
    //   // avg_rtt = 0.99*avg_rtt + 0.01*total_time;
    // // }
    // avg_rtt = 100000;
    // array_to_leds_delay(DINO, avg_rtt);
    // Serial.print("measure.RangeMilliMeter: ");
    // Serial.print(measure.RangeMilliMeter);
    // Serial.print(", ");
    // Serial.print("total_time: ");
    // Serial.println(total_time);
  // }
}

void profile_sensor_without_detect() {
   if (sensor.readRangeContinuousMillimeters() < 2000) {
      Serial.print("half rotate: ");
      Serial.println(micros() - start_time);
      detected = true;
      array_to_leds(DINO);
      start_time = micros();
      // array_to_leds_delay(DINO, 1);
    }
}

// void test_sensor() {
//   VL53L0X_RangingMeasurementData_t measure;
//   lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout! this is where measure gets set
//   if (arm_distance(measure)) {
//     Serial.println("you won");
//   }
//   Serial.println("nvm");
// }

// bool arm_distance(VL53L0X_RangingMeasurementData_t measure) {
//   return (measure.RangeMilliMeter >= 110 && measure.RangeMilliMeter <= 140); // 110 to 130 based on testing --> was a bit inaccurate so maybe 90 - 140?
//   // or maybe i just do either number or out of range, depends on if there's anything else that can interfere
// }

void array_to_leds_delay(int array[][COLUMN], long total_time) {
  long delay =  ((1/4)/ROW)*total_time; // (1/4th of display/image width)*rotation time
  unsigned int binary = 0;
  for(int i = 0; i < COLUMN; i++) {
    binary = col_to_bin(array, i);
    tlcmanager.broadcast().off_pattern(0xFFFF);
    tlcmanager.broadcast().on_pattern(binary);
  }
  tlcmanager.broadcast().off_pattern(0xFFFF);
  // delayMicroseconds(100000);
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


// OLD CODE FROM EXAMPLE ---------------------------------------------------------------------------------------------------------
void do_blinks(TLC59116 &t) {
  static unsigned long blink_cycle_start = 0;
  const static int On_Time = 3; // elapsed from beginning of cycle. empirical to keep led on very short
  const static int Off_Time = On_Time + 150; // elapsed from beginning of cycle

  unsigned long now = millis();
  if (now - blink_cycle_start > Off_Time) {
    digitalWrite(7,LOW); // for sink, low is on
    digitalWrite(13,HIGH);
    // do individual addressing to prove it works (not just broadcast)
    blink_cycle_start = now;
    }
  else if (now - blink_cycle_start > On_Time) {
    digitalWrite(7,HIGH);
    digitalWrite(13,LOW);
    }
  Serial.println("Device count: ");
  Serial.println(tlcmanager.device_count());

  BlinkTracking::update(now);
  }

void do_triangles(TLC59116 &t) {
  static long triangle_value = 1;
  triangle(triangle_value, 0,255, 1,1, false);
  // not doing individual addressing for triangle.
  tlcmanager.broadcast().pwm(4,7,abs(triangle_value));
  // For pin 6 triangle (source not sink)
  analogWrite(6, abs(triangle_value));
  }

#define debug(msg)
#define debugln(msg)

void triangle(long &state, int min, int max, int up_inc, int down_inc, boolean once) {
  // Set state to 1 to start running. We will set state to 0 when we want to stop!
  // debug(min);debug('<');debug(state);debug('<');debug(max);debug(' ');
  if (state < 0) {
    // debug("N<");debug(min);debug(' ');
    state += down_inc;
    if (state >= -min) {
      if (once) {
        /// debug("once'd\n");debugln();
        state = 0; // mark not running
        return;
        // return true; // done
        }
      else {
        /// debug("~peak'd ");debugln();
        state = min;
        }
      }
    }
  else {
    // debug("N>");debug(min);debug(' ');
    if (state == max) {
      /// debug("peak'd ");debugln();
      state = -max; // was max last time, so wrap
      }
    else {
      state += up_inc;
      if (state > max) {
        state = max; // clip to max, and allow it for next time
        }
      }
    }
  if (state == 0) state = 1; // don't allow 0
  // debug("=");debug(state);debugln();
  // return false;
  }
