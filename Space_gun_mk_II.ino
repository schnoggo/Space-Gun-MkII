// Space Gun MkII
// Lon Koenig 2016

#include "shared.h" // bring in libraries, declare globals for libraries, define macros


// Set up the IO pins:
#define NEO_PIXEL_DATA_PIN 3
#define NEOPIXEL_WHITE_DATA_PIN 8
#define TRIGGER_PIN 0

//  SoftwareSerial Rest, RX & TX pins:
#define SFX_RST 5
#define SFX_TX 6
#define SFX_RX 7


// Other constants:
#define LED_14_I2C_ADDR 0x70
#define MODE_DEMO 8


#define MODE_CONFIG	   0
#define MODE_STAR_WARS 2
#define MODE_TREK      3
#define MODE_DEMO      8
#define MODE_DIAMOND   4
#define MODE_LON01     1
#define MODE_NRG_BLADE 5




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


#define ANIM14_DEMO 0
#define ANIM14_RAND 1
#define ANIM14_NUM  2
#define ANIM14_MSG  3
#define ANIM14_SPIN 4
#define ANIM14_MARQUEE  5

//Global vars:
int trigger_reading;
int last_trigger_reading = -1;
uint8_t trigger_value = 0;
unsigned long trigger_timer = 0;

#define TRIGGER_SAMPLE_RATE 20
#define TRIGGER_RANGE 1024/3

uint8_t current_orientation = 99;
uint8_t last_orientation = 100; // start with them different
unsigned long last_orientation_change = 0;
unsigned long last_accel_reading = 0;
#define ORIENT_FORWARD 0
#define ORIENT_GROUND  1
#define ORIENT_TIP_OUT 2
#define ORIENT_TIP_IN  3
#define ORIENT_SKY     4
#define ORIENT_INVERT  5
#define ORIENT_GUNPLAY 6
#define ORIENT_BOUNDARY 7

#define ACCEL_SAMPLE_RATE 30
#define ORIENT_HISTORY_SIZE 10
uint8_t orientation_history[10];
#define POSSIBLE_ORIENTATIONS 8
uint8_t orientation_counts[POSSIBLE_ORIENTATIONS];


byte current_mode = MODE_DEMO;
byte prev_mode = 0;


// combine the timers and current playing animation (and frame counter) into struct at some point
byte ring_animation = ANIM_DEMO;


unsigned long timer_white = 0;
unsigned long timer_audioFX = 0;
// set up timers:
#define SEG14 0
#define RINGS 1
#define WHITE_PIX 2
#define AUDIO 3
AnimTimer anim_timers[4];



uint8_t acc; // generic 8-bit global to avoid memory thrashing
uint32_t c = 0; // generic global color
uint32_t ring_anim_color=0;
byte alpha_squence_index = '!';


uint32_t white_range = (256*4)-1;
int white_direction = WHITE_ANIM_STEP_SIZE;
signed long white_anim_step = 0;
uint8_t white_rgb[4]; // declared global so we're not constantly allocating this


uint16_t ring_anim_step = 0;



// 14-segment stuff:
uint8_t seg14_anim = ANIM14_DEMO;
// for some reson, couldn't just declare these in shared.h, so assigning here:
SoftwareSerial  ss = SoftwareSerial(SFX_TX, SFX_RX);
Adafruit_Soundboard  sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);

void setup() {
    #ifdef PRDBG
      Serial.begin(9600);
    #endif

    randomSeed(analogRead(2)); // Pin 2 should be floating
    InitAudioFX(); // set up sound board and serial
    InitAccel(); // set up accelerometer:
    InitSeg14(); // set up 14-segment display
    InitNeoPixels(); // set up NeoPixels


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


// trigger:
  boolean trigger_change = UpdateTriggerState();
  if (trigger_change){
    dprint("trigger: ");
    dprintln(trigger_reading);
    if (2 == trigger_reading){
      StartAudioFX(4);
    } else {
      StopAudioFX();
    }
}

  boolean orientation_changed = UpdateAccelData();

  if (orientation_changed){
    PrintOrientation();
    StartRingAnimation(ANIM_DEMO);
    if (ORIENT_FORWARD == current_orientation) {
      StartSeg14Animation(ANIM14_RAND);
    } else {
      StartSeg14Animation(ANIM14_DEMO);
    }

  }

}

void ServiceLights(){
  unsigned long now = millis();
  byte neopixel_dirty = false; // Don't update NeoPixel string unless we have to


  // Alphanumeric display:
    AnimateSeg14(now);



  // NeoPixelRings:
    neopixel_dirty = AnimateRings(now) || neopixel_dirty;


  // White NeoPixels:
  if (timer_white < now){
    neopixel_dirty = AnimateWhite(now) || neopixel_dirty;
  } // timer

  UpdateNeopixels(neopixel_dirty);
}


void ServiceSound(){
  unsigned long now = millis(); // for timing-based sounds
  if (timer_audioFX < now){
    AnimateAudioFX(now);
  }

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
      case MODE_CONFIG:
      break;

      case MODE_LON01:

      break;


      case MODE_DEMO:

      break;

  }
 prev_mode =   current_mode; // and set the current mode
  }
}
