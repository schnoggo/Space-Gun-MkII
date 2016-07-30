// Space Gun MkII
// Lon Koenig 2016

// switches:
#define PRDBG
#define USE_NEOPIXEL
#define USE_SEG14
#define USE_ACCEL


// Libraries:
#ifdef PRDBG
#include <Wire.h>
#endif

#include <SoftwareSerial.h>
#ifdef USE_NEOPIXEL
#include <Adafruit_NeoPixel.h> // Neopixels. We're using GRB and Warm White
#endif

#ifdef USE_SEG14
#include "Adafruit_LEDBackpack.h" // for 14-segment displau (adafru.it/---)
#include "Adafruit_GFX.h" // Adafruit's general Arduino graphics lib
#endif

#ifdef USE_ACCEL
#include <Adafruit_MMA8451.h> // Adafruit accelerometer breakout
#endif


#include "Adafruit_Soundboard.h" // Adafruit audio FX board



#ifdef PRDBG
#define dprint(ccc) Serial.print(ccc);
#define dprintln(ccc) Serial.println(ccc);
#else
#define dprint(ccc) ""
#define dprintln(ccc) ""
#endif



// Set up the IO pins:
#define NEO_PIXEL_DATA_PIN 3
#define NEOPIXEL_WHITE_DATA_PIN 8
#define TRIGGER_PIN 0

//  SoftwareSerial to RX & TX
#define SFX_RST 5
#define SFX_TX 6
#define SFX_RX 7



// Other constants:
#define LED_14_I2C_ADDR 0x70
#define MODE_ADMIN 0
#define MODE_PEW 1
#define MODE_DEMO 8

#define ANIM_DEMO 0
#define ANIM_STANDBY 1
#define ANIM_FIRE_QUICK 2
#define ANIM_FIRE_LONG 3
#define ANIM_FIRE_BLAST 4


#define RING_START 0
#define RING_END 23
#define WHITE_START 24
#define WHITE_END 25
#define WHITE_ANIM_STEP_SIZE 50


#ifdef USE_NEOPIXEL
// Initialize some library objects:
// Neopixels:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(WHITE_END+1, NEO_PIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel strip = Adafruit_NeoPixel(WHITE_END, NEOPIXEL_WHITE_DATA_PIN, NEO_GRB + NEO_KHZ800);
#endif

#ifdef USE_SEG14
// 14-segment LED:
Adafruit_AlphaNum4 LED_14_seg = Adafruit_AlphaNum4();
#endif

#ifdef USE_ACCEL
// accelerometer:
Adafruit_MMA8451 accelerometer = Adafruit_MMA8451();
#endif



// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.



//Global vars:
int trigger_reading;
int last_trigger_reading = -1;

uint8_t current_orientation= -99;
uint8_t last_orientation = 0; // start with them different

byte current_mode = 254;
byte prev_mode = 0;

byte ring_animation = ANIM_DEMO;


unsigned long timer_14seg = 0;
unsigned long timer_rings = 0;
unsigned long timer_white = 0;
byte acc;
uint32_t c = 0;

byte alpha_squence_index = '!';


uint32_t white_range = (256*4)-1;
int white_direction = WHITE_ANIM_STEP_SIZE;
signed long white_anim_step = 0;
uint8_t white_rgb[4]; // declared global so we're not constantly allocating this


uint16_t ring_anim_step = 0;




// Use the Adafruit Sensor Event library to normalize reading sensor inputs:
  sensors_event_t sensor_event;




  // audio board / Software serial:
  SoftwareSerial ss = SoftwareSerial(SFX_TX, SFX_RX);
  Adafruit_Soundboard sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);


void setup() {
    #ifdef PRDBG
      Serial.begin(9600);
    #endif





    if (!sfx.reset()) {
      dprint("soundboard not found");
    } else {
      dprint("SFX board found");
    }
    // setup accelerometer:
    InitAccel();
      InitSeg14();
      InitNeoPixels();

/*
digitalWrite(SFX_RST, LOW);
pinMode(SFX_RST, OUTPUT);
delay(10);
pinMode(SFX_RST, INPUT);
delay(1000); // give a bit of time to 'boot up'

*/
/*
  ListSFXFiles();
  sfx.playTrack(3);

*/


}

void loop() {
  UpdateMode();

// in the off chance no function is servicing he heartbeat:
  ServiceLights();
  ServiceSound();
  ServiceSensors();
}
void ServiceSensors(){
/*
  Update global sensor values
*/

int temp_trigger = analogRead(TRIGGER_PIN); // might be bouncing

    accelerometer.getEvent(&sensor_event);

  /* Display the results (acceleration is measured in m/s^2) */
//  dprint("X: \t"); dprint(sensor_event.acceleration.x); dprint("\t");
//  dprint("Y: \t"); dprint(sensor_event.acceleration.y); dprint("\t");
//  dprint("Z: \t"); dprint(sensor_event.acceleration.z); dprint("\t");

   /* Get the orientation of the sensor */
  current_orientation = accelerometer.getOrientation();
/*
  switch (o) {
    case MMA8451_PL_PUF:
      dprintln("Portrait Up Front");
      break;
    case MMA8451_PL_PUB:
      dprintln("Portrait Up Back");
      break;
    case MMA8451_PL_PDF:
      dprintln("Portrait Down Front");
      break;
    case MMA8451_PL_PDB:
      dprintln("Portrait Down Back");
      break;
    case MMA8451_PL_LRF:
      dprintln("Landscape Right Front");
      break;
    case MMA8451_PL_LRB:
      dprintln("Landscape Right Back");
      break;
    case MMA8451_PL_LLF:
      dprintln("Landscape Left Front");
      break;
    case MMA8451_PL_LLB:
      dprintln("Landscape Left Back");
      break;
    }
  dprintln();


  dprintln("m/s^2 ");

 */

}

void ServiceLights(){
  unsigned long now = millis();
  byte neopixel_dirty = false;
// Alphanumeric display:

  if (timer_14seg < now){
    AnimateSeg14(now);
  } // timer


    // NeoPixelRings:
    if (timer_rings < now){
      neopixel_dirty = AnimateRings(now) || neopixel_dirty;
    } // timer



    // White Pixels:
  if (timer_white < now){
    neopixel_dirty = AnimateWhite(now) || neopixel_dirty;
  }

  UpdateNeopixels(neopixel_dirty);
}


void ServiceSound(){


}
void safe_delay(unsigned long duration){
  unsigned long fake_delay_end = millis() + duration;
  while( millis() < fake_delay_end){

    ServiceLights();
    ServiceSound();
    ServiceSensors();
  }


}

void UpdateMode(){
// Uses some globals
// current_mode, prev_mode
  if (current_mode != prev_mode){
    switch(current_mode){
      case MODE_ADMIN:
      break;

      case MODE_PEW:

      break;


      case MODE_DEMO:

      break;

  }
  current_mode = prev_mode; // and set the current mode
  }
}




// sfx.reset()
// uint8_t n = readnumber();
//   if (! sfx.playTrack(n) ) {
//
// or by name:
// char name[20];
// if (! sfx.playTrack(name) ) {
//
/*
uint16_t v;
if (! (v = sfx.volUp()) ) {
  dprintln("Failed to adjust");
} else {
  dprint("Volume: "); dprintln(v);
}
*/

// if (! (v=sfx.volDown()) ) {
