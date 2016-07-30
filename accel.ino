// accelerometer


void InitAccel(){
  #ifdef USE_ACCEL
  dprintln("Adafruit MMA8451 test!");
  if (! accelerometer.begin()) {
    dprintln("Couldnt start");
  }
  dprintln("MMA8451 found!");
  accelerometer.setRange(MMA8451_RANGE_2_G);
  //accelerometer.setRange(MMA8451_RANGE_4_G);
  //accelerometer.setRange(MMA8451_RANGE_8_G);
  #endif
}
