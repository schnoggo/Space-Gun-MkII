// Space Gun MkII
// Lon Koenig 2016

// switches:
//#define PRDBG

// Libraries:
#ifdef PRDBG
#include <Wire.h>
#endif

#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h> // Neopixels. We're using GRB and Warm White
#include "Adafruit_LEDBackpack.h" // for 14-segment displau (adafru.it/---)
#include "Adafruit_GFX.h" // Adafruit's general Arduino graphics lib
#include <Adafruit_MMA8451.h> // Adafruit accelerometer breakout
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


// Initialize some library objects:
// Neopixels:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(WHITE_END+1, NEO_PIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel strip = Adafruit_NeoPixel(WHITE_END, NEOPIXEL_WHITE_DATA_PIN, NEO_GRB + NEO_KHZ800);


// 14-segment LED:
Adafruit_AlphaNum4 LED_14_seg = Adafruit_AlphaNum4();
// accelerometer:
Adafruit_MMA8451 accelerometer = Adafruit_MMA8451();



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

// setup accelerometer:
    Serial.begin(9600);



    if (!sfx.reset()) {
      dprint("soundboard not found");
    } else {
      dprint("SFX board found");
    }


/*
  dprintln("Adafruit MMA8451 test!");
    if (! accelerometer.begin()) {
    dprintln("Couldnt start");
 //   while (1);
  }
  dprintln("MMA8451 found!");
  accelerometer.setRange(MMA8451_RANGE_2_G);
//accelerometer.setRange(MMA8451_RANGE_4_G);
//accelerometer.setRange(MMA8451_RANGE_8_G);




  // Setup Neopixels:
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Setup 14-segment display:
   LED_14_seg.begin(LED_14_I2C_ADDR);  // pass in the address
*/

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
    if (alpha_squence_index == '!'){
      //reset
      LED_14_seg.clear();
      LED_14_seg.writeDisplay();
    } else {
      LED_14_seg.writeDigitAscii(0, alpha_squence_index);
      LED_14_seg.writeDigitAscii(1, alpha_squence_index+1);
      // LED_14_seg.writeDigitAscii(2, i+2);
      //  LED_14_seg.writeDigitAscii(3, i+3);
      LED_14_seg.writeDisplay();
    }
    alpha_squence_index++;
    if (alpha_squence_index >= 'z'){
      alpha_squence_index = '!';
    }
    timer_14seg = millis()+300;
  } // timer


    // NeoPixelRings:
    if (timer_rings < now){
    //if (false){
      switch(ring_animation){
        case ANIM_DEMO:
          if (ring_anim_step > 255) {
            ring_anim_step = 0;
          } else {
            ring_anim_step++;
          }
          uint16_t i;
          for(i=RING_START; i<=RING_END; i++) {
            strip.setPixelColor(i, Wheel((i+ ring_anim_step ) & 255));
          }
          neopixel_dirty = true;
          timer_rings = now +30;

        break;

        case ANIM_STANDBY:
        break;

        case ANIM_FIRE_LONG:
        break;

        case ANIM_FIRE_BLAST:
        break;
      }
    } // timer



// White Pixels:

//  if (timer_white < now){
if (false){


/*
0: 0,  0,  0,
50: 13,  13,  12,
100: 25,  25,  25,
150: 38,  38,  37,
200: 50,  50,  50,
250: 63,  63,  62,
300: 75,  75,  75,
350: 88,  88,  87,
400: 100,  100,  100,
450: 113,  113,  112,
500: 125,  125,  125,
550: 138,  138,  137,
600: 150,  150,  150,
650: 163,  163,  162,
700: 175,  175,  175,
750: 188,  188,  187,
800: 200,  200,  200,
850: 213,  213,  212,
900: 225,  225,  225,
950: 238,  238,  237,
1000: 250,  250,  250,
1023: 0,  0,  0,
973: 244,  243,  243,
923: 231,  231,  231,
873: 219,  218,  218,
823: 206,  206,  206,
773: 194,  193,  193,
723: 181,  181,  181,
673: 169,  168,  168,
623: 156,  156,  156,
573: 144,  143,  143,
523: 131,  131,  131,
473: 119,  118,  118,
423: 106,  106,  106,
373: 94,  93,  93,
323: 81,  81,  81,
273: 69,  68,  68,
223: 56,  56,  56,
173: 44,  43,  43,
123: 31,  31,  31,
73: 19,  18,  18,
23: 6,  6,  6,
1023: 0,  0,  0,
973: 244,  243,  243,
923: 231,  231,  231,
873: 219,  218,  218,
823: 206,  206,  206,
*/
 dprint(white_anim_step);
  dprint(": ");
      // spread possible values of 0 -768 across 3 pixels
    for(byte i=0; i<3; i++) { // rgb
      acc = white_anim_step/4;
      byte d = white_anim_step%4;
      if (i < d){
        acc++;
        dprintln();
      }
      white_rgb[i] = acc;
     dprint(acc);
      dprint(",  ");
    }

  //  c = strip.Color(white_rgb[0],white_rgb[1], white_rgb[2], white_rgb[3]);
   // c = strip.Color(white_anim_step/3,0,0);
  // acc = white_anim_step%255;
     c = strip.Color(white_rgb[0],white_rgb[1], white_rgb[2]);
    strip.setPixelColor(WHITE_START , c);
    //strip.setPixelColor(WHITE_START -1 , c);
    neopixel_dirty = true;



    white_anim_step = white_anim_step + white_direction;
    if (white_anim_step >= white_range){
      white_anim_step = white_range;
      white_direction = 0 - WHITE_ANIM_STEP_SIZE;
    }

    if (white_anim_step <= 0){
      white_anim_step = 0;
      white_direction = WHITE_ANIM_STEP_SIZE;
    }

  timer_white = millis()+200;
  } // timer




if (neopixel_dirty){
strip.show();
}


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



  // Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  int red;
  int green;
  int blue;
  if(WheelPos < 85) {
   // return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);

   green = 0;
   blue = WheelPos * 3;
   red = 255 - blue;

  } else {
  if(WheelPos < 170) {
    WheelPos -= 85;
   // return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
   red = 0;
   green = WheelPos * 3;
   blue = 255-green;

  } else {
  WheelPos -= 170;
  // return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  blue = 0;
  red = WheelPos *3;
  green = 255 - red;
  }


  }

  // adjust brightness:
  red = red *.05;
  green = green *.05;
  blue = blue * .05;
  return strip.Color(red, green, blue);

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
