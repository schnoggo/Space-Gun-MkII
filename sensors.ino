// accelerometer


void InitAccel(){
// globals: 
//   accelerometer
//   sensor_event
  #ifdef USE_ACCEL
  accelerometer = Adafruit_MMA8451();


  dprintln("Adafruit MMA8451 test!");
  if (! accelerometer.begin()) {
    dprintln("Couldnt start");
  }
  dprintln("MMA8451 found!");
  accelerometer.setRange(MMA8451_RANGE_2_G);
  //accelerometer.setRange(MMA8451_RANGE_4_G);
  //accelerometer.setRange(MMA8451_RANGE_8_G);





 /* Display the results (acceleration is measured in m/s^2) */
//  dprint("X: \t"); dprint(sensor_event.acceleration.x); dprint("\t");
//  dprint("Y: \t"); dprint(sensor_event.acceleration.y); dprint("\t");
//  dprint("Z: \t"); dprint(sensor_event.acceleration.z); dprint("\t");

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




/*
#define ORIENT_FORWARD 0
#define ORIENT_GROUND  1
#define ORIENT_TIP_OUT 2
#define ORIENT_TIP_IN  3
#define ORIENT_SKY     4
#define ORIENT_INVERT  5
#define ORIENT_GUNPLAY 6
*/


  boolean retVal = false;
  unsigned long now = millis(); 
  #ifdef USE_ACCEL
  
  if ( (now - last_orientation_change ) > ACCEL_SAMPLE_RATE){
  
  
  
  // Tell the library to query the accelerometer
  accelerometer.getEvent(&sensor_event);
  dprint("X: \t"); dprint(sensor_event.acceleration.x); dprint("  ");
  dprint("Y: \t"); dprint(sensor_event.acceleration.y); dprint("  ");
  dprint("Z: \t"); dprintln(sensor_event.acceleration.z); dprint("  ");
  last_orientation_change =  now;
  
  }
/*

  uint8_t new_orientation = accelerometer.getOrientation();
  if ( current_orientation != new_orientation){
    if ( (now - last_orientation_change ) > ORIENTATION_DEBOUNCE_TIME){
      last_orientation = current_orientation;
      current_orientation = new_orientation;
      last_orientation_change = now;
      retVal = true;
    }
  }
*/
  #endif
  return retVal;
}


void PrintOrientation(){
  #ifdef USE_ACCEL
  /*
     dprint(current_orientation);
     dprint(": ");
  switch (current_orientation) {
    case MMA8451_PL_PUF:
      dprintln("Portrait Up Front");
      break;
    case MMA8451_PL_PUB:
      dprintln("Portrait Up Back");
      break;
    case MMA8451_PL_PDF:
      dprintln("Portrait Down Front");
      break;
    case MMA8451_PL_PDB:
      dprintln("Portrait Down Back");
      break;
    case MMA8451_PL_LRF:
      dprintln("Landscape Right Front");
      break;
    case MMA8451_PL_LRB:
      dprintln("Landscape Right Back");
      break;
    case MMA8451_PL_LLF:
      dprintln("Landscape Left Front");
      break;
    case MMA8451_PL_LLB:
      dprintln("Landscape Left Back");
      break;
    }
*/
#endif


}