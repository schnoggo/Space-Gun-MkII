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


void AnimateSeg14(unsigned long now){
  /*
  demo:
  globals:
  alpha_squence_index - current character
  anim_timers

  */
#ifdef  USE_SEG14
    if (TimerUp(SEG14, now)){
    if (alpha_squence_index == '!'){
      //reset
      LED_14_seg.clear();
      LED_14_seg.writeDisplay();
    } else {
      LED_14_seg.writeDigitAscii(0, alpha_squence_index);
      LED_14_seg.writeDigitAscii(1, alpha_squence_index+1);
      LED_14_seg.writeDisplay();
    }
    alpha_squence_index++;
    if (alpha_squence_index >= 'z'){
      alpha_squence_index = '!';
    }

     SetTimer( SEG14, 150);
    }
  #endif
}
