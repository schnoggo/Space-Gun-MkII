// 14 segment display initializations and animations


void InitSeg14(){
#ifdef  USE_SEG14
// Setup 14-segment display:
 LED_14_seg.begin(LED_14_I2C_ADDR);  // pass in the address

#endif
}


void AnimateSeg14(unsigned long now){
  /*
  demo:
  globals:
  alpha_squence_index - current character


  */
#ifdef  USE_SEG14

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

      timer_14seg = millis()+300;

  #endif
}
