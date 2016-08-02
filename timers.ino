void ClearTimer(uint8_t this_timer){

  anim_timers[this_timer].frame = 0;
  anim_timers[this_timer].next_frame_time = 0;
  anim_timers[this_timer].active = true;
  
}

boolean TimerUp(uint8_t this_timer, unsigned long now){
  boolean retVal = false;
  if (anim_timers[this_timer].active){
  
    if (anim_timers[this_timer].next_frame_time < now){
      retVal = true;
    }
  }
  return retVal;
}

void SetTimer(uint8_t this_timer, unsigned long when){
  anim_timers[this_timer].next_frame_time  = millis()+when;
}