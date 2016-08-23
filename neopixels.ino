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
  // anim_timers[RINGS].anim_id
  // inputs:
  // anim_num which animation to play

// locals:
  int red;
  int green;
  int blue;


     anim_timers[RINGS].anim_id = anim_num;
      ResetAnimation(RINGS);
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
        // RingSolid(ring_anim_color); // actually handled in the animation section
       break;
    break;

  case ANIM_RING_STANDBY:
  case ANIM_RING_BACK_TO_FRONT:

    anim_timers[RINGS].anim_id = anim_num;
  break;

  case ANIM_RING_F2BWIDE:
    anim_timers[RINGS].anim_id = anim_num;
    SetTimer( RINGS, 170);
  break;


  case ANIM_RING_FIRE_LOW:
  case ANIM_RING_FIRE_HI:
    ring_anim_color = strip.Color(0, 0 , 0);
    anim_timers[RINGS].anim_id = anim_num;
    RingSolid(ring_anim_color);
  break;


  }


}




byte AnimateRings(  unsigned long now){
  /*
  Globals:
    anim_timers[RINGS].anim_id // W
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
      this_frame = GetTimerFrame(RINGS); //int 
      switch(anim_timers[RINGS].anim_id){
        case ANIM_RING_STANDBY:
          for(pixel_index = RING_START; pixel_index <= RING_END;  pixel_index++) {
            strip.setPixelColor(pixel_index, Wheel((pixel_index+ ( this_frame % 255) ) & 255));
          }
          neopixel_dirty = true;
           SetTimer( RINGS, 30);
          AdvanceTimerFrame(RINGS);
        break;

        case ANIM_RING_BACK_TO_FRONT:
        
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

          ResetAnimation(RINGS);
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
          RingSolid(ring_anim_color);
          neopixel_dirty = true;
          SetTimer( RINGS, 3000);

        break;


        case ANIM_RING_FIRE_LOW:
        this_frame = GetTimerFrame(RINGS); //int

          ring_anim_color = strip.Color((this_frame+2)*15,(this_frame+2)*15,(this_frame+2)*15);
          RingSolid(ring_anim_color);
          neopixel_dirty = true;
          if (this_frame <12){
            AdvanceTimerFrame(RINGS);
            SetTimer( RINGS, 15);
          } else {
            RingSolid(0);
            SetTimer( RINGS, 30000);
          }
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


void StartWhiteAnimation(byte anim_num){
  dprint("StartWhiteAnimation");
  dprintln(anim_num);
  anim_timers[WHITE_PIX].anim_id = anim_num;
  int w1, w2, w3;
  uint16_t i;
    switch (anim_num){
      case ANIM_WHITE_PULSE:
      w1 = 0;
      w2 = 0;
      w3 = 0;

      break;
      case ANIM_WHITE_RANDOGLOW:
        for( i=0; i<9; i++ ) {
          white_pixel_colors[i] = random(255);
        }
    
      break;

      default:
      w1 = 0;
      w2 = 0;
      w3 = 0;
    }

    white_anim_color = strip.Color(w1, w2, w3);
   ResetAnimation(WHITE_PIX);

}

byte AnimateWhite(  unsigned long now){
  byte neopixel_dirty = false;
  unsigned int this_frame;
  uint32_t t_color;
  int rand_step;
  uint8_t i, j;
  #ifdef USE_NEOPIXEL
  if (TimerUp(WHITE_PIX, now)){
    this_frame = GetTimerFrame(WHITE_PIX); //int
    switch(anim_timers[WHITE_PIX].anim_id){
      case ANIM_WHITE_PULSE:
        if ( this_frame % 2 == 0 ){
            white_anim_color =  strip.Color(00, 15, 0);
        } else {
              white_anim_color =  strip.Color(0, 0, 0);
        }
        WhiteSolid(white_anim_color);
        neopixel_dirty =  true;

        SetTimer( WHITE_PIX, 300);
        AdvanceTimerFrame(WHITE_PIX);
      break;

      case ANIM_WHITE_BACK_TO_FRONT:
        if ( this_frame < 3) {
          for(uint16_t i=0; i<3; i++) {
            if (i == this_frame) {
              white_anim_color = strip.Color(15, 15, 15);
            } else {
              white_anim_color = 0;
            }
            SetWhitePixel(i, white_anim_color);
          }
          neopixel_dirty = true;
          SetTimer( WHITE_PIX, 70);
          AdvanceTimerFrame(WHITE_PIX);
        } else {
          if ( 3 == this_frame){
            WhiteSolid(0);
            neopixel_dirty = true;
            SetTimer( WHITE_PIX, 20000);
            AdvanceTimerFrame(WHITE_PIX);
          }

        }
      break;
      
      case ANIM_WHITE_RANDOGLOW:
       for( i=0; i<3; i++ ) {
        for( j=0; j<3; j++){   
           rand_step = 6 - random(13);
            if ((white_pixel_colors[i] + rand_step) > 255){
              white_pixel_colors[i] = 255;
            } else {
              if ((white_pixel_colors[i] + rand_step) < 0){
                 white_pixel_colors[i] = 0;
              } else {
                white_pixel_colors[i] = white_pixel_colors[i] + rand_step;
            }
          }
          }
            SetWhitePixel(i, strip.Color(
              white_pixel_colors[(i*3)]/15.0,
              white_pixel_colors[(i*3 + 1)]/15.0,
              white_pixel_colors[(i*3 + 2)]/15.0
            ));
      
      }
       SetTimer( WHITE_PIX, 60);
      neopixel_dirty =  true;
      break;

    }
  }















  /*
      if (this_frame < 255){
        white_anim_color =  strip.Color(this_frame, this_frame, this_frame);
      } else {
          white_anim_color =  strip.Color(511-this_frame, 511-this_frame, 511-this_frame);
      }
        neopixel_dirty =  true;
        for(uint16_t i=WHITE_START; i<=WHITE_END; i++) {
            strip.setPixelColor(i, white_anim_color);
        }

        if (this_frame < 512){
          SetTimer( WHITE_PIX, 20);
          AdvanceTimerFrame(WHITE_PIX);
        } else {
          ResetAnimation(WHITE_PIX);
        }
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


/*
#define RING_START 0 // first color neopixel in rings
#define RING_END 23 // last color neopixel in rings
#define WHITE_START 24 // first white (W,W,W) Neopixel
#define WHITE_END 29
*/

void RingSolid(uint32_t c){
  for(uint8_t i=RING_START; i<=RING_END; i++) {
      strip.setPixelColor(i, c);
  }
}

void WhiteSolid(uint32_t color){
  for(uint16_t i=WHITE_START; i<=WHITE_END; i++) {
    strip.setPixelColor(i, color);
  }
}

// to get white pixel from ordinate:
// pixel 1 = 2-x. pixel 2 = x+3
void SetWhitePixel(uint8_t x, uint32_t color){
// inputs:
// x = ordintate (0-2) 0 is rear of gun
// color = NeoPixel packed color
  uint8_t ord = (WHITE_START + 2 - x);
  strip.setPixelColor(  ord, color);
  ord =(WHITE_START + 3 + x);
  strip.setPixelColor(  ord , color);
}



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
