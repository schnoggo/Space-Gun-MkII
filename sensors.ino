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


  boolean retVal = false;
  #ifdef USE_ACCEL
  unsigned long now = millis(); 
// figue out a throttling mechanism.... maybe?
  
  float xV = sensor_event.acceleration.x;
  float yV = sensor_event.acceleration.y;
  float zV = sensor_event.acceleration.z;
  
  // Tell the library to query the accelerometer
  accelerometer.getEvent(&sensor_event);
  /*
  dprint("X: \t"); dprint(xV); dprint("  ");
  dprint("Y: \t"); dprint(yV); dprint("  ");
  dprint("Z: \t"); dprintln(zV); dprint("  ");
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
uint8_t return_orientation = ORIENT_BOUNDARY;
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
  if (current_orientation != return_orientation) { // only need to update is position changed
    if (ORIENT_BOUNDARY != return_orientation){ // don't change if in boundary zones
      if ( (now - last_orientation_change ) > ORIENTATION_DEBOUNCE_TIME){ //don't report transient changes - this might change for "GUNPLAY" result
        if ( current_orientation != return_orientation){
          if ( (now - last_orientation_change ) > ORIENTATION_DEBOUNCE_TIME){
            last_orientation = current_orientation;
            current_orientation = return_orientation;
            last_orientation_change = now;
            retVal = true;
          }
        }
      }
    }
  }

  #endif
  return retVal;
}


void PrintOrientation(){
  #ifdef USE_ACCEL

     dprint(current_orientation);
     dprint(": ");
  switch (current_orientation) {
  
  
  #define ORIENT_FORWARD 0
#define ORIENT_GROUND  1
#define ORIENT_TIP_OUT 2
#define ORIENT_TIP_IN  3
#define ORIENT_SKY     4
#define ORIENT_INVERT  5
#define ORIENT_GUNPLAY 6



    case ORIENT_FORWARD:
      dprintln("Forward");
      break;
    case ORIENT_GROUND:
      dprintln("Ground");
      break;
    case ORIENT_TIP_OUT:
      dprintln("Roll Out");
      break;
    case ORIENT_TIP_IN:
      dprintln("Roll In");
      break;
    case ORIENT_SKY:
      dprintln("Sky");
      break;
    case ORIENT_INVERT:
      dprintln("Invert");
      break;
    case ORIENT_GUNPLAY:
      dprintln("Waving");
      break;
    }

#endif


}