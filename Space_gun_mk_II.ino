// Space Gun MkII
// Lon Koenig 2016-2017

#include "shared.h" // bring in libraries, declare globals for libraries, define macrose
// Set up the IO pins:
#define NEO_PIXEL_DATA_PIN 3
#define NEOPIXEL_WHITE_DATA_PIN 8
#define TRIGGER_PIN 1
#define AUDIO_SENSE 8

//  SoftwareSerial Rest, RX & TX pins:
#define SFX_RST 11
#define SFX_TX 10
#define SFX_RX 9


// Other constants:
#define LED_14_I2C_ADDR 0x70
#define MODE_DEMO 8


#define MODE_CONFIG	   0
#define MODE_LON01     1
#define MODE_STAR_WARS 2
#define MODE_TREK_TOS      3
#define MODE_DEMO      4
#define MODE_DIAMOND   5
#define MODE_NRG_BLADE 6
#define MODE_TREK_TNG 7


char* mode_names[] = {
  "CONIG   ",
  "DEFAULT   ",
  "STAR WARS   ",
  "TREK-TOS   ",
  "DEMO POSITION    ",
  "DIAMOND   ",
  "SABER   ",
  "TREK-TNG   "
};



#define ANIM_RING_DEMO 0
#define ANIM_RING_STANDBY 1
#define ANIM_RING_FLASH_MED 2
#define ANIM_RING_FLASH_FAST 3
#define ANIM_RING_BACK_TO_FRONT 4
#define ANIM_RING_F2BWIDE 5
#define ANIM_RING_SOUNDBOARD 6
#define ANIM_RING_FLASH_SLOW 7
#define ANIM_RING_PULSE_SLOW 8
#define ANIM_RING_PULSE_MED 9
#define ANIM_RING_SPARKLE 10




#define SND_STARTUP 0
#define SND_TOS_01 1
#define SND_TOS_02 2
#define SND_TOS_03 3
#define SND_TNG_01 4
#define SND_TNG_02 5
#define SND_TNG_03 6
#define SND_SW_01 7
#define SND_SW_02 8
#define SND_SW_03 9
#define SND_NERD_01 10
#define SND_NERD_02 11
#define SND_NERD_03 12
#define SND_NERD_04 13
#define SND_NERD_05 14
#define SND_NERD_06 15
#define SND_NERD_07 16
#define SND_NERD_08 17
#define SND_NONE 0xff





uint8_t neopixel_slices[15] = {
  0x09, 0xff,
  0x0a, 0x08,
  0x0b, 0x07,
  0x00, 0x06,
  0x01, 0x05,
  0x02, 0x04,
  0x03, 0xff
};
#define ANIM_FIRE_QUICK 0
#define ANIM_FIRE_LONG 1
#define ANIM_FIRE_BLAST 2


#define RING_START 0 // first color neopixel in rings
#define RING_END 23 // last color neopixel in rings
#define WHITE_START 24 // first white (W,W,W) Neopixel
#define WHITE_END 29 // last white neopixel
#define NOSE_START 30 //first RGB pixel in nose cone
#define NOSE_END 36 // last RGB pixel in nose cone
#define WHITE_ANIM_STEP_SIZE 50
#define RING_OFFSET_LEFT 0
#define RING_OFFSET_RIGHT 0


#define ANIM14_DEMO 0
#define ANIM14_RAND 1
#define ANIM14_NUM  2
#define ANIM14_MSG  3
#define ANIM14_SPIN 4
#define ANIM14_MARQUEE  5


#define ANIM_WHITE_BLINK 0
#define ANIM_WHITE_SOLID 1
#define ANIM_WHITE_BACK_TO_FRONT 2
#define ANIM_WHITE_RANDOGLOW 3
#define ANIM_WHITE_PULSE 4
#define ANIM_WHITE_PULSE_FAST 5

#define ANIM_NOSE_DEMO 0


//Global vars:
uint8_t trigger_position;
uint8_t last_trigger_position = 0xff;
boolean new_trigger_pull = false;
unsigned long trigger_timer = 0;

#define TRIGGER_SAMPLE_RATE 120

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
#define ORIENT_UP_LEFT 8
#define ORIENT_UP_RIGHT 9

#define ACCEL_SAMPLE_RATE 30
#define ORIENT_HISTORY_SIZE 10
uint8_t orientation_history[10];
#define POSSIBLE_ORIENTATIONS 8
uint8_t orientation_counts[POSSIBLE_ORIENTATIONS];


byte current_mode = MODE_DEMO;
byte prev_mode = 0;




unsigned long timer_audioFX = 0;
// set up timers:
#define SEG14 0
#define RINGS 1
#define WHITE_PIX 2
#define NOSE 3
#define AUDIO 4
AnimTimer anim_timers[5];

// defined named for animation groups below
#define A_DEMO 0
#define A_BLASTER1 1
#define A_BLASTER2 2
#define A_BLASTER3 3
#define A_CONFIG 4
#define PULSE_BLAST 5
#define A_SOUNDBOARD 6
#define A_TOS_01 7
#define A_TOS_02 8
#define A_TOS_03 9
#define A_TNG_01 10
#define A_TNG_02 11
#define A_TNG_03 12
#define A_SW_01 13
#define A_SW_02 14
#define A_SW_03 15
#define NO_ANIM 0xff


AnimGroup animations[16]{
  // ring  anim ID, // sound  #,// white  ID, 14-segment
  { ANIM_RING_DEMO,          SND_NONE,   ANIM_WHITE_BLINK,     ANIM14_RAND,    ANIM_NOSE_DEMO}, // 00 A_DEMO
  { ANIM_RING_FLASH_MED,      01 ,        ANIM_WHITE_PULSE,     ANIM14_MSG,     ANIM_NOSE_DEMO},  // 01 A_BLASTER1 (SW)_
  { ANIM_RING_BACK_TO_FRONT, 02,         ANIM_WHITE_RANDOGLOW, ANIM14_DEMO,    ANIM_NOSE_DEMO},  // 02 A_BLASTER2
  { ANIM_RING_FLASH_FAST,       03,         ANIM_WHITE_PULSE,     ANIM14_RAND,   ANIM_NOSE_DEMO},  // 03 A_BLASTER3
  { ANIM_RING_DEMO,          SND_NONE,   ANIM_WHITE_RANDOGLOW, ANIM14_NUM,    ANIM_NOSE_DEMO}, // 04  A_CONFIG
  { ANIM_RING_F2BWIDE,       03,         ANIM_WHITE_BACK_TO_FRONT, ANIM14_MSG, ANIM_NOSE_DEMO}, // 05 PULSE_BLAST
  { ANIM_RING_SOUNDBOARD,    SND_NONE,   ANIM_WHITE_RANDOGLOW,  0xff,          ANIM_NOSE_DEMO}, // 06 A_SOUNDBOARD

  { ANIM_RING_FLASH_MED,      SND_TOS_01, ANIM_WHITE_PULSE, ANIM14_MSG, ANIM_NOSE_DEMO}, // 07 Trek TOS 01
  { ANIM_RING_FLASH_FAST,       SND_TOS_02, ANIM_WHITE_PULSE, ANIM14_MSG, ANIM_NOSE_DEMO}, // 08 Trek TOS 02
  { ANIM_RING_SPARKLE,         SND_TOS_03, ANIM_WHITE_PULSE, ANIM14_MSG, ANIM_NOSE_DEMO}, // 09 Trek TOS 03

  { ANIM_RING_FLASH_FAST,       SND_TNG_02, ANIM_WHITE_PULSE, ANIM14_MSG, ANIM_NOSE_DEMO}, // 10 Trek TNG 01
  { ANIM_RING_PULSE_MED,      SND_TNG_01, ANIM_WHITE_BACK_TO_FRONT, ANIM14_MSG, ANIM_NOSE_DEMO}, // 11 Trek TNG 02
  { ANIM_RING_F2BWIDE, SND_TNG_03, ANIM_WHITE_BACK_TO_FRONT, ANIM14_MSG, ANIM_NOSE_DEMO}, // 12 Trek TNG 03

  { ANIM_RING_FLASH_MED,       SND_SW_02, ANIM_WHITE_PULSE, ANIM14_MSG, ANIM_NOSE_DEMO}, // 13 Star Wars 01
  { ANIM_RING_FLASH_MED,      SND_SW_01, ANIM_WHITE_PULSE, ANIM14_MSG, ANIM_NOSE_DEMO}, // 14 Star Wars 02
  { ANIM_RING_FLASH_FAST, SND_SW_03, ANIM_WHITE_PULSE, ANIM14_MSG, ANIM_NOSE_DEMO} // 15 Star Wars 03

//  { 0xff, 03, ANIM_WHITE_BACK_TO_FRONT, ANIM14_MSG} // PULSE_BLAST
};




uint8_t acc; // generic 8-bit global to avoid memory thrashing
uint32_t c = 0; // generic global color
uint32_t ring_anim_color=0;
uint32_t white_anim_color=0;
int white_anim_dir = 1;
byte alpha_squence_index = '!';
uint8_t white_pixel_colors[9]; //3 sets of "rgb" (actually WWW)
#define MAX_WHITE_GLOW  60


uint16_t ring_anim_step = 0;

uint8_t selected_mode = MODE_LON01;

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

    SetNewMode(current_mode); // uses global current_mode


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
  also handles mode changes
*/
  UpdateTriggerState(); // no results - just updates state in the background
  uint8_t trigger = GetTriggerPosition(false); // not full-auto, returns [ 0 | 1 | 2 ]
  boolean orientation_changed = UpdateAccelData();
  if (orientation_changed){
    PrintOrientation();
  }


  switch (current_mode) {

      case MODE_CONFIG:
        if ( 0 < trigger ){
          switch (current_orientation){

            case ORIENT_GROUND:
              SetNewMode(selected_mode); // Exit config and go to new mode
            break;

            case ORIENT_TIP_IN:
            case ORIENT_TIP_OUT:
            case ORIENT_FORWARD:
              selected_mode++;
              if (selected_mode > MODE_TREK_TNG) {selected_mode = MODE_LON01;}
              dprint(F("config:selected_mode: "));
              dprintln(selected_mode);
            //  SetSeg14Value(selected_mode);
          SetSeg14Msg(mode_names[selected_mode]);


              //  PlayAnimation(A_CONFIG);
            //  StartSeg14Animation(ANIM14_NUM);
          StartSeg14Animation(ANIM14_MSG);
            break;
          }
        }
    break;


    case MODE_LON01:
    case MODE_STAR_WARS:
    case MODE_TREK_TNG:
    case MODE_TREK_TOS:
      if ( 0 < trigger ){
        switch(current_orientation){
          case ORIENT_FORWARD:
            if (false == IsFXPlaying()){
              PlayAnimation(GetGunAnimation(current_mode, trigger));
            }
          break;

         case ORIENT_TIP_IN:
         case ORIENT_TIP_OUT:
           if (false == IsFXPlaying() ){
              PlayAnimation(GetGunAnimation(current_mode, 3));
          }
         break;

         case ORIENT_GROUND:
           if ( 02 == trigger){
            SetNewMode(MODE_CONFIG); // aim at ground pull trigger to enter config
           }
         break;

         }
     }
    break; //MODE_LON01, MODE_TREK_TOS, MODE_STAR_WARS, MODE_TREK_TNG


    case MODE_DEMO:
      // to exit demo, aim at sky and pull trigger
      if (orientation_changed){
        StartRingAnimation(ANIM_RING_DEMO);
      }

      if( ORIENT_GROUND == current_orientation){
        if ( 0 < trigger ){
          SetNewMode(MODE_LON01);
        }
      }
    break;

    case MODE_DIAMOND:
      if ( (0 < trigger) ){
          //  PlayAnimation(A_BLASTER1);
        //  PlayAnimation(GetGunAnimation(current_mode));

        switch(current_orientation){
          case ORIENT_FORWARD:
            InteruptAudioFX(16);
          break;

          case ORIENT_SKY:
            InteruptAudioFX(11);
          break;

          case ORIENT_TIP_IN:
          InteruptAudioFX(14);
          break;

          case ORIENT_TIP_OUT:
          InteruptAudioFX(13);
          break;

          case ORIENT_UP_LEFT:
          InteruptAudioFX(15);
          break;

          case ORIENT_UP_RIGHT:
            InteruptAudioFX(16);
          break;

          case ORIENT_GROUND:
             SetNewMode(MODE_CONFIG); // aim at ground pull trigger to enter config
          break;

        }

      }
    break; // MODE_DIAMOND

    }

}

uint8_t GetGunAnimation(byte which_mode, byte gun_index){
  //Inputs:
  // current global mode
  //
  // Return:
  // Group animation number suitable for PlayAnimation()

  uint8_t retVal = A_BLASTER1;
  switch(which_mode){
    case MODE_LON01:
    //  retVal = A_BLASTER3;
  //  retVal = PULSE_BLAST;
    retVal = A_TOS_01 - 1 + gun_index;
    break;

    case MODE_STAR_WARS:
    //  retVal = A_BLASTER1;
    retVal = A_SW_01 - 1 + gun_index;
    break;

    case MODE_TREK_TOS:
    retVal = A_TOS_01 - 1 + gun_index;
  //    retVal = A_BLASTER2;

    break;

    case MODE_TREK_TNG:
    retVal = A_TNG_01 - 1 + gun_index;

    break;

  }
  /*
  dprint(F("animation: "));
  dprint(retVal);
  dprint(F("  mode: "));
  dprintln(which_mode);
  */
  return retVal;

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
