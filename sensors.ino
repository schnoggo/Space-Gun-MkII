// accelerometer


void InitAccel(){
// globals:
//   accelerometer
//   sensor_event
  #ifdef USE_ACCEL
  accelerometer = Adafruit_MMA8451();


  dprintln(F("Adafruit MMA8451 test!"));
  if (! accelerometer.begin()) {
    dprintln(F("Couldnt start"));
  }
  dprintln(F("MMA8451 found!"));
  accelerometer.setRange(MMA8451_RANGE_2_G);
  //accelerometer.setRange(MMA8451_RANGE_4_G);
  //accelerometer.setRange(MMA8451_RANGE_8_G);

// clear the history (used for debounce)
      for(uint8_t i=0; i<ORIENT_HISTORY_SIZE; i++) {
        orientation_history[i] = ORIENT_BOUNDARY;
      }




 /* Display the results (acceleration is measured in m/s^2) */
//  dprint(F("X: \t")); dprint(sensor_event.acceleration.x); dprint(F("\t"));
//  dprint(F("Y: \t")); dprint(sensor_event.acceleration.y); dprint(F("\t"));
//  dprint(F("Z: \t")); dprint(sensor_event.acceleration.z); dprint(F("\t"));

#endif

}

boolean UpdateAccelData(){
//
// globals:
//   accelerometer - Adafruit_MMA8451
//   sensor_event - sensors_event_t
//   current_orientation
//   last_orientation
//   last_orientation_change
// last_accel_reading


  boolean retVal = false;
  #ifdef USE_ACCEL
  unsigned long now = millis();
  // throttle readings:
  if (last_accel_reading + ACCEL_SAMPLE_RATE < now){

    float xV = sensor_event.acceleration.x;
    float yV = sensor_event.acceleration.y;
    float zV = sensor_event.acceleration.z;

    // Tell the library to query the accelerometer
    accelerometer.getEvent(&sensor_event);
    /*
    dprint(F("X: \t")); dprint(xV); dprint(F("  "));
    dprint(F("Y: \t")); dprint(yV); dprint(F("  "));
    dprint(F("Z: \t")); dprintln(zV); dprint(F("  "));
   */



  /*
    X	Y	Z
  TIP_OUT	-2< x < +2	-6 < y < 5	z<-6
  TIP_IN	-2< x < +2	-6 < y < 5	6<z
  INVERT	-2< x < +2	7 < y	   -6 < z < 6

  FORWARD	-2< x < +2	y < -7	 -6 < z < 6
  GROUND	6 < x	      y < -7	 -6 < z < 6
  SKY	    x<-6        y < -7	 -6 < z < 6

  */
  uint8_t return_orientation = ORIENT_BOUNDARY; // default result is no result
  if (zV > -6) {
    if (zV < 6){
      if (yV >7){ // probably inverted
         if ( (xV > -2) && (xV < 2)){
          return_orientation = ORIENT_INVERT;
         }
      } else {
        if (xV < -6){ return_orientation = ORIENT_SKY; }
        if (xV > 6 ){ return_orientation = ORIENT_GROUND; }
        if ( (xV > -2) && (xV < 2)){
          return_orientation = ORIENT_FORWARD;
        }
      }
    } else {
    // only case where z> 6
      if ( (xV > -2) && (xV < 2)){
        return_orientation = ORIENT_TIP_IN;
      }
    }

  } else {
    // only one case where z < -6
    if ( (xV > -2) && (xV < 2)){
      return_orientation = ORIENT_TIP_OUT;
    }

  }

// now check explicitly for upper-left
  if (
      ((xV < -5) && (xV > -7))
      &&
      ((yV < -3) && (yV > -6))
      &&
      ((zV > 5) && (zV < 7))
     ) {
    return_orientation = ORIENT_UP_LEFT;
  }

  // now check explicitly for upper-right
    if (
        ((xV < -6) && (xV > -8))
        &&
        ((yV < -2) && (yV > -5))
        &&
        ((zV < -2.5) && (zV > -6))
       ) {
      return_orientation = ORIENT_UP_RIGHT;
    }

  // at this point, return_orientation is our interpretation fo the current readings.
  // Now, deboounce that and return a sensible result to the caller
  RecordOrientation(return_orientation);
  return_orientation = GetDebouncedOrientation();
    if (ORIENT_BOUNDARY != return_orientation){ // don't change if in boundary zones
      if (current_orientation != return_orientation) { // only need to update if position changed
        last_orientation = current_orientation;
        current_orientation = return_orientation;
        last_orientation_change = now;
        retVal = true;
        }
      }
  last_accel_reading = now;
  } // throttle
  #endif
  return retVal;
}

void RecordOrientation(uint8_t new_o){
// add this new orientation to the history buffer:
// not using a pointer and wrapping logic, just shifting the buffer:
uint8_t i;
      for(i=0; i < ORIENT_HISTORY_SIZE - 1; i++) {
        orientation_history[i] =  orientation_history[i+1];
      }
    orientation_history[ORIENT_HISTORY_SIZE - 1] = new_o;

}

uint8_t GetDebouncedOrientation(){

// returns the most seen orientation in the last  ORIENT_HISTORY_SIZE samples
//
// clear the counts:
uint8_t i;
uint8_t winning_slot = 254;
uint8_t winning_value = 0;
uint8_t retVal = ORIENT_BOUNDARY;
      // clear the counts:
      for(i=0; i < POSSIBLE_ORIENTATIONS; i++) {
        orientation_counts[i] = 0;
      }

      // count the orientations & find a winner:
      for(i=0; i < ORIENT_HISTORY_SIZE - 1; i++) {
        orientation_counts[orientation_history[i]]++;
        if (orientation_counts[orientation_history[i]] > winning_value){
          winning_slot = i;
          winning_value = orientation_counts[orientation_history[i]];
        }
      }

  if (winning_value > 0){
    retVal = orientation_history[winning_slot];
  }
  return retVal;

}

void PrintOrientation(){
  #ifdef USE_ACCEL

     dprint(current_orientation);
     dprint(F(": "));
  switch (current_orientation) {

    case ORIENT_FORWARD:
      dprintln(F("Forward"));
      break;
    case ORIENT_GROUND:
      dprintln(F("Ground"));
      break;
    case ORIENT_TIP_OUT:
      dprintln(F("Roll Out"));
      break;
    case ORIENT_TIP_IN:
      dprintln(F("Roll In"));
      break;
    case ORIENT_SKY:
      dprintln(F("Sky"));
      break;
    case ORIENT_INVERT:
      dprintln(F("Invert"));
      break;
    case ORIENT_GUNPLAY:
      dprintln(F("Waving"));
      break;
    case ORIENT_UP_LEFT:
      dprintln(F("Up Left"));
    break;
    case ORIENT_UP_RIGHT:
      dprintln(F("Up Right"));
    break;
    }

#endif


}


void InitTrigger(){
  // globals:
  trigger_position = 0;
  new_trigger_pull = 0;

}
/*
 * Monitors trigger input and updated position values
 * Called as a background function
 */
void UpdateTriggerState(){
// globals:
//   trigger_timer - use to debounce the readings a little
//   trigger_position - current trigger position
//   new_trigger_pull
  unsigned long now = millis();
  // maybe add a throttle?
  if (trigger_timer < now ){

    int temp_trigger = analogRead(TRIGGER_PIN); // might be bouncing
    //temp_trigger = temp_trigger/(TRIGGER_RANGE); // reduce to position
    // convert full 0 - 1023 to [ 0 | 1 | 2]


    if (temp_trigger < 100 ){ //  100 for FSR // 600 for resistors/switches
      temp_trigger = 0;
    } else {
      if ( temp_trigger < 768  ){ // 768 for real sensor. 400 for switch
          temp_trigger = 1;
      } else {
        temp_trigger = 2;
      }
    }



    if (temp_trigger !=  trigger_position){
        dprint("trigger: ");
        dprintln(temp_trigger);
    // trigger has changed position:
      if (trigger_position < temp_trigger){
        new_trigger_pull = true;
      } else {
        new_trigger_pull = false;
      }
      trigger_position = temp_trigger;
    }
    trigger_timer =  now + TRIGGER_SAMPLE_RATE;
  }
}
/*
* Returns NEW trigger position
* 0 = trigger not pulled (or unchanging)
* otherwise new trigger position [ 1 | 2 ]
* parm auto-repeat: clears the new_trigger_pull flag
*/
uint8_t GetTriggerPosition(boolean full_auto){
  // globals:
  //   trigger_position - current trigger position
  //   new_trigger_pull
  uint8_t retVal = 0;
  if (new_trigger_pull){
    retVal = trigger_position;

    if (full_auto){
    //  new_trigger_pull =  false;
    //last_trigger_position = 0;
    }
    new_trigger_pull = false;
    dprint(F("GetTriggerPosition: "));
    dprintln(retVal);
  }

  return retVal;
}
