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




#define ANIM_RING_DEMO 0
#define ANIM_RING_STANDBY 1

#define ANIM_FIRE_QUICK 0
#define ANIM_FIRE_LONG 1
#define ANIM_FIRE_BLAST 2


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
byte ring_animation = ANIM_RING_DEMO;


unsigned long timer_audioFX = 0;
// set up timers:
#define SEG14 0
#define RINGS 1
#define WHITE_PIX 2
#define AUDIO 3
AnimTimer anim_timers[4];

AnimGroup animations[3]{
  // ring  ID, // sound  #,// white  ID, 14-segment
  { ANIM_RING_DEMO , 0xff, 0xff, ANIM14_RAND},
  { ANIM_RING_STANDBY , 2, 0xff, ANIM14_MSG},
  { ANIM_RING_DEMO , 1, 0xff, ANIM14_RAND},
};
#define A_DEMO 0
#define A_BLASTER1 1
#define A_BLASTER2 2
#define NO_ANIM 0xff



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
#define SEG14_BUFFER_SIZE 22
char seg14_buffer[SEG14_BUFFER_SIZE + 1];



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

// in the off chance no function is servicing he heartbeat:
  ServiceLights();
  ServiceSound();
  ServiceSensors();
}



void ServiceSensors(){
/*
  Update global sensor values
*/
  boolean trigger_change = UpdateTriggerState();
  boolean orientation_changed = UpdateAccelData();
  switch (current_mode) {
    case MODE_CONFIG:
      if( (ORIENT_SKY == current_orientation) && (02 == trigger_reading)){
        SetNewMode(MODE_LON01);
      }
    break;


    case MODE_LON01:
      switch(current_orientation){
        case ORIENT_FORWARD:
          if (trigger_change && (02 == trigger_reading)){
            PlayAnimation(A_BLASTER1);
          }
        break;

        case ORIENT_GROUND:
          if (02 == trigger_reading){
            SetNewMode(MODE_CONFIG);
          }
        break;
      }

    break; //MODE_LON01


    case MODE_DEMO:
      if( (ORIENT_SKY == current_orientation) && (02 == trigger_reading)){
        SetNewMode(MODE_LON01);
      }
    break;


  }


/*
// trigger:
  boolean trigger_change = UpdateTriggerState();
  if (trigger_change){
    dprint("trigger: ");
    dprintln(trigger_reading);
    if (2 == trigger_reading){
      StartAudioFX(2);
    } else {
      StopAudioFX();
    }
}

  boolean orientation_changed = UpdateAccelData();

  if (orientation_changed){
    PrintOrientation();
    StartRingAnimation(ANIM_RING_DEMO);
    if (ORIENT_FORWARD == current_orientation) {
    //  SetSeg14Value(10);
    //  StartSeg14Animation(ANIM14_NUM);
      SetSeg14Msg("WE  LOVE  ADAFRUIT   ");
      StartSeg14Animation(ANIM14_MSG);
    } else {
      StartSeg14Animation(ANIM14_DEMO);
    }

  }
*/
}

void ServiceLights(){
  unsigned long now = millis();
  byte neopixel_dirty = false; // Don't update NeoPixel string unless we have to


  // Alphanumeric display:
    AnimateSeg14(now);



  // NeoPixelRings:
    neopixel_dirty = AnimateRings(now) || neopixel_dirty;


  // White NeoPixels:

    neopixel_dirty = AnimateWhite(now) || neopixel_dirty;


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
