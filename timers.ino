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
      StartRingAnimation(t_anim);
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

void SetNewMode(uint8_t new_mode){
  prev_mode = current_mode;
  current_mode = new_mode;

}
