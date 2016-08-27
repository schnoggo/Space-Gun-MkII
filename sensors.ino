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
    }

#endif


}


void InitTrigger(){
  trigger_reading = 0;
  last_trigger_reading = -1;
  trigger_value =  0;

}

boolean UpdateTriggerState(){
  boolean retVal = false;
  unsigned long now = millis();
  // maybe add a throttle?
  if (trigger_timer < now ){

    int temp_trigger = analogRead(TRIGGER_PIN); // might be bouncing
     // dprintln(temp_trigger);

    temp_trigger = temp_trigger/(TRIGGER_RANGE);
    if (temp_trigger !=  trigger_reading){
      last_trigger_reading = trigger_reading;
      trigger_reading = temp_trigger;

      retVal =  true;
    }
    trigger_timer =  now + TRIGGER_SAMPLE_RATE;
  }
  // globals:
  // int trigger_reading;
  // int last_trigger_reading = -1;
  // uint8_t trigger_value

  return retVal;

}
