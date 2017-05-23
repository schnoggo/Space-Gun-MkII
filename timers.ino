void ClearTimer(uint8_t this_timer){

  anim_timers[this_timer].anim_id = 0xff;
  ResetAnimation(this_timer);

}

void ResetAnimation(uint8_t this_timer){
  anim_timers[this_timer].frame = 0;
  anim_timers[this_timer].next_frame_time = 0;
  anim_timers[this_timer].active = true;
}


boolean TimerUp(uint8_t this_timer, unsigned long now){
  // returns if it is time to display the next
  // frame of the specified timer
  boolean retVal = false;
  if (anim_timers[this_timer].active){

    if (anim_timers[this_timer].next_frame_time < now){
      retVal = true;
    }
  }
  return retVal;
}

void SetTimer(uint8_t this_timer, unsigned long when){
  // Sets the time of the next frame for the specified timer
  // inpputs:
  // which timer
  // when

  anim_timers[this_timer].next_frame_time  = millis()+when;
}

void SetTimerAnimation(uint8_t this_timer, byte anim_id){
  anim_timers[this_timer].anim_id  = anim_id;
}

byte GetTimerAnimation(uint8_t this_timer){
  return anim_timers[this_timer].anim_id;

}

void AdvanceTimerFrame(uint8_t this_timer){
  anim_timers[this_timer].frame++;
}

unsigned int GetTimerFrame(uint8_t this_timer){
  return anim_timers[this_timer].frame;
}


void PlayAnimation(uint8_t anim_num){
/*
  dont play anything if passed animation it NO_ANIM
uint8_t ring; // ring animation sequence ID
uint8_t sound; // sound track #
uint8_t white; // white LED animation ID
uint8_t seg14; // 14-segment display animation ID
*/
  if ( NO_ANIM != anim_num){
    uint8_t t_anim;
    t_anim = animations[anim_num].ring;
    if ( NO_ANIM != t_anim) {
      StartRingAnimation(t_anim);
    }

    t_anim = animations[anim_num].sound;
    if ( NO_ANIM != t_anim) {
      StartAudioFX(t_anim);
    }

    t_anim = animations[anim_num].white;
    if ( NO_ANIM != t_anim) {
      StartWhiteAnimation(t_anim);
    }

    t_anim = animations[anim_num].nose;
    if ( NO_ANIM != t_anim) {
      StartNoseAnimation(t_anim);
    }

    t_anim = animations[anim_num].seg14;
    if ( NO_ANIM != t_anim) {
        if (ANIM14_MSG == t_anim){
            SetSeg14Msg("WE  LOVE  ADAFRUIT   ");
        }
      StartSeg14Animation(t_anim);
    }
  }
}

void SetNewMode(byte new_mode){
  // globals:
  // selected_mode - currently selected mode in config mode
  // prev_mode - the previous, duh

  dprint("set new mode: ");
  dprintln(new_mode);
  prev_mode = current_mode;
  current_mode = new_mode;


  switch (new_mode) {
    case MODE_CONFIG:
    SetSeg14Value(selected_mode);
      PlayAnimation(A_CONFIG);
    break;

    case MODE_LON01:
      PlayAnimation(A_BLASTER1);
    break;

    case MODE_DEMO:
      PlayAnimation(A_BLASTER2);
    break;

    case MODE_DIAMOND:
      PlayAnimation(A_SOUNDBOARD);
    break;
  }

}
