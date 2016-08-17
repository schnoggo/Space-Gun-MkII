// bring in libraries, declare globals for libraries, define macros

// switches:
#define PRDBG
#define USE_NEOPIXEL
#define USE_SEG14
#define USE_ACCEL
#define USE_AUDIOFX

// Libraries:
#include <Wire.h>


// Neopixels:
#ifdef USE_NEOPIXEL
#include <Adafruit_NeoPixel.h> // Neopixels. We're using GRB and Warm White
Adafruit_NeoPixel strip; // declare our global NeoPixel object
// Adafruit_NeoPixel strip = Adafruit_NeoPixel(WHITE_END, NEOPIXEL_WHITE_DATA_PIN, NEO_GRB + NEO_KHZ800);
#endif
// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


// 14-segment LED:
#ifdef USE_SEG14
#include "Adafruit_LEDBackpack.h" // for 14-segment displau (adafru.it/---)
#include "Adafruit_GFX.h" // Adafruit's general Arduino graphics lib
Adafruit_AlphaNum4 LED_14_seg; // declare our global AlphaNum4 object
#endif

#ifdef USE_ACCEL
#include <Adafruit_MMA8451.h> // Adafruit accelerometer breakout
#endif

#ifdef USE_ACCEL
// accelerometer:
Adafruit_MMA8451 accelerometer; // declare our global MMA8451 (accelerometer) object
sensors_event_t sensor_event; // declare our globalsensor event object
#endif


#ifdef USE_AUDIOFX
#include <SoftwareSerial.h>
#include "Adafruit_Soundboard.h" // Adafruit audio FX board
  // audio board / Software serial:
//  SoftwareSerial ss =  SoftwareSerial(9, 10); // declare global
 // Adafruit_Soundboard sfx; // declare global
#endif



// timer objects for animations:
typedef struct {
  byte id;
  unsigned int frame; // current "frame" of animation sequence
  unsigned long next_frame_time; // time of next update time (in millis)
  boolean active; // whether this animation is active
} AnimTimer;


typedef struct {
  uint8_t ring; // ring animation sequence ID
  uint8_t sound; // sound track #
  uint8_t white; // white LED animation ID
  uint8_t seg14; // 14-segment display animation ID
} AnimGroup;

// macros:
// create Serial.print macros:
#ifdef PRDBG
#define dprint(ccc) Serial.print(ccc);
#define dprintln(ccc) Serial.println(ccc);
#else
#define dprint(ccc) ""
#define dprintln(ccc) ""
#endif
