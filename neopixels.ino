// neopixel initializations and animations
//
// animations use these globals:
void InitNeoPixels(){
// Global:
// strip
  // Setup Neopixels:
  #ifdef USE_NEOPIXEL
  strip = Adafruit_NeoPixel(WHITE_END+1, NEO_PIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  #endif
}

void StartRingAnimation(byte anim_num){
  // globals:
  // ring_animation

// locals:
  int red;
  int green;
  int blue;


    ring_animation = anim_num;
// stop any existing animations

// start the new animation
  switch (anim_num){
  case ANIM_RING_DEMO:
    // for now, don't check for anim type - just set it to demo

    switch (current_orientation){
      case ORIENT_FORWARD:
        red = 255; green = 20; blue = 0;
        break;

      case ORIENT_SKY:
        red = 00 ; green = 00; blue = 255;
        break;

      case ORIENT_GROUND:
        red = 00 ; green = 255; blue = 20;
      break;

      case ORIENT_TIP_IN:
        red = 30 ; green = 30; blue = 00;
      break;

      case ORIENT_TIP_OUT:
        red = 00 ; green = 30; blue = 30;
      break;


      default:
         red = 10 ; green = 10; blue = 10;

       }
         ring_anim_color = strip.Color(red, green, blue);
       break;
  break;

  case ANIM_RING_STANDBY:
    ring_animation = ANIM_RING_STANDBY;
  break;

  case ANIM_RING_BACK_TO_FRONT:
  case ANIM_RING_F2BWIDE:
    ring_animation = anim_num;
  break;

  case ANIM_RING_FIRE_LOW:
  case ANIM_RING_FIRE_HI:
     ring_anim_color = strip.Color(0, 0 , 0);
  break;


  }

   ClearTimer(RINGS);
}




byte AnimateRings(  unsigned long now){
  /*
  Globals:
    ring_animation // W
    strip
    ring_anim_step

    Return neopixel_dirty - neopixels have changed
      false - no changes made to neopixels

anim_timers[this_timer].frame = 0;
  */
  byte neopixel_dirty = false;
  byte pixel_index;
  uint8_t strip_pos;
  unsigned int this_frame;

  #ifdef USE_NEOPIXEL

    if (TimerUp(RINGS, now)){

      switch(ring_animation){
        case ANIM_RING_STANDBY:
          if (ring_anim_step > 255) {
          ring_anim_step = 0;
          } else {
          ring_anim_step++;
          }

          for(pixel_index = RING_START; pixel_index <= RING_END;  pixel_index++) {
          strip.setPixelColor(pixel_index, Wheel((pixel_index+ ring_anim_step ) & 255));
          }
          neopixel_dirty = true;
        break;

        case ANIM_RING_BACK_TO_FRONT:
        this_frame = GetTimerFrame(RINGS); //int
          for(pixel_index = 0; pixel_index <= 6;  pixel_index++) {
            for (uint8_t j = 0; j <2; j++){
              strip_pos = neopixel_slices[(pixel_index*2) + j];
              if (strip_pos < 0xff){
                neopixel_dirty = true;
                // right-hand ring
                if (this_frame == pixel_index) {
                  strip.setPixelColor(strip_pos, strip.Color(128,128,128));
                } else {
                  strip.setPixelColor(strip_pos, strip.Color(0,0,0)); // make dark
                }

                // left-hand ring
                if (this_frame == (6 - pixel_index)) {
                  strip.setPixelColor(strip_pos + 12, strip.Color(128,128,128));
                } else {
                  strip.setPixelColor(strip_pos + 12, strip.Color(0,0,0)); // make dark
                }
              }
          }
        }
        if (this_frame < 10){
          AdvanceTimerFrame(RINGS);
        } else {

          ClearTimer(RINGS);
        }

          SetTimer( RINGS, 50);
        break;


        case ANIM_RING_F2BWIDE:
        this_frame = GetTimerFrame(RINGS); //int
          for(pixel_index = 0; pixel_index <= 6;  pixel_index++) {
            for (uint8_t j = 0; j <2; j++){
              strip_pos = neopixel_slices[(pixel_index*2) + j];
              if (strip_pos < 0xff){
                neopixel_dirty = true;
                // right-hand ring
                if (
                  (this_frame == pixel_index)
                  ||
                  (this_frame == (pixel_index-1))
                ) {
                  strip.setPixelColor(strip_pos, strip.Color(128,128,128));
                } else {
                  strip.setPixelColor(strip_pos, strip.Color(0,0,0)); // make dark
                }

                // left-hand ring
                if (
                  (this_frame == (6 - pixel_index))
                ||
                  (this_frame == (5 - pixel_index))
                ) {
                  strip.setPixelColor(strip_pos + 12, strip.Color(128,128,128));
                } else {
                  strip.setPixelColor(strip_pos + 12, strip.Color(0,0,0)); // make dark
                }
              }
          }
        }

          AdvanceTimerFrame(RINGS);
          SetTimer( RINGS, 25);
        break;


        case ANIM_RING_DEMO:
          NeoWipe(ring_anim_color, 0);
          neopixel_dirty = true;
          SetTimer( RINGS, 3000);

        break;


        case ANIM_RING_FIRE_LOW:
          ring_anim_color = strip.Color(30,30,30);
          neopixel_dirty = true;
          SetTimer( RINGS, 3000);
        break;


        case ANIM_RING_FIRE_HI:
          ring_anim_color = strip.Color(128,128,128);
          neopixel_dirty = true;
          SetTimer( RINGS, 3000);
        break;
/*


      #define ANIM_RING_FIRE_LOW 2
      #define ANIM_RING_FIRE_HI 3
*/
      }

    }
    return neopixel_dirty;
    #endif

}


byte AnimateWhite(  unsigned long now){
  byte neopixel_dirty = false;
  #ifdef USE_NEOPIXEL
  if (TimerUp(WHITE_PIX, now)){
    SetTimer( WHITE_PIX, 3000);
}


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

/*
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


    } // timer
*/
    #endif
    return neopixel_dirty;

}


void UpdateNeopixels(byte dirty){
  #ifdef USE_NEOPIXEL
    if(dirty){
      strip.show();
    }
  #endif
}


  #ifdef USE_NEOPIXEL
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
#endif


// Fill the dots one after the other with a color
void NeoWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      if ( 0 != wait ){
        strip.show();
        safe_delay(wait);
      }
  }
  if (0 == wait){
 //   strip.show();
  }
}



uint8_t NormalizeRingPos(uint8_t realPos){

  while (realPos < 0) { realPos += 16;}
  while (realPos > 15) { realPos -= 16; }
  return realPos;
}
