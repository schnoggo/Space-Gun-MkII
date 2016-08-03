// 14 segment display initializations and animations


void InitSeg14(){
// uses global: LED_14_seg
#ifdef  USE_SEG14
// Setup 14-segment display:
  LED_14_seg = Adafruit_AlphaNum4();
  LED_14_seg.begin(LED_14_I2C_ADDR);  // pass in the address
  ClearTimer(SEG14);


#endif
}

/*#define ANIM14_DEMO 0
#define ANIM14_RAND 1
#define ANIM14_NUM  2
#define ANIM14_MSG  3
#define ANIM14_SPIN 4
#define ANIM14_MARQUEE  5

*/
void StartSeg14Animation(byte anim_num){
  seg14_anim = anim_num; // global
  ClearTimer(SEG14);
  switch (seg14_anim) {
    case ANIM14_NUM:
   LED_14_seg.writeDigitAscii(0, seg14_buffer[0]);
    LED_14_seg.writeDigitAscii(1, seg14_buffer[1]);
    LED_14_seg.writeDisplay();
  /*  LED_14_seg.writeDigitAscii(0, "9");
    LED_14_seg.writeDigitAscii(1, "9");
    dprint("anim14: ");
    dprintln("ANIM14_NUM");
*/
    break;
    default:
    dprint("anim14: ");
    dprintln(anim_num);

  }
}

void SetSeg14Value(uint8_t num){
  itoa (num,seg14_buffer,10);
}

void AnimateSeg14(unsigned long now){
  /*
  demo:
  globals:
  alpha_squence_index - current character
  anim_timers

  */
  #ifdef  USE_SEG14
    if (TimerUp(SEG14, now)){
      char temp_char;
    switch (seg14_anim) {
      case ANIM14_DEMO:
          LED_14_seg.writeDigitAscii(0, alpha_squence_index);
          LED_14_seg.writeDigitAscii(1, alpha_squence_index+1);
          LED_14_seg.writeDisplay();
          alpha_squence_index++;
          if (alpha_squence_index >= 'z'){
            alpha_squence_index = '!';
          }

         SetTimer( SEG14, 150);

      break;

      case ANIM14_RAND:
      temp_char = random(64) + 64;
      LED_14_seg.writeDigitAscii(0, temp_char);
      temp_char = random(64) + 64;
      LED_14_seg.writeDigitAscii(1, temp_char);
      LED_14_seg.writeDisplay();

        SetTimer( SEG14, 100);
      break;

      case ANIM14_NUM:
        SetTimer( SEG14, 4000);

      break;

    }
  } // timer

  #endif
}
