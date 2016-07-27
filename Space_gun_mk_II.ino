// Space Gun MkII
// Lon Koenig 2016

// Libraries:
#include <Wire.h>
#include <Adafruit_NeoPixel.h> // Neopixels. We're using GRB and Warm White





#include "Adafruit_LEDBackpack.h" // for 14-segment displau (adafru.it/---)
#include "Adafruit_GFX.h" // Adafruit's general Arduino graphics lib
#include <Adafruit_MMA8451.h> // Adafruit accelerometer breakout


// Set up the IO pins:
#define NEO_PIXEL_DATA_PIN 3

// Other constants:
#define LED_14_I2C_ADDR 0x70


#ifdef __AVR__
  #include <avr/power.h>
#endif

// Initialize some library objects:
// Neopixels:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(32, NEO_PIXEL_DATA_PIN, NEO_GRB + NEO_KHZ800);
// 14-segment LED:
Adafruit_AlphaNum4 LED_14_seg = Adafruit_AlphaNum4();
// accelerometer:
Adafruit_MMA8451 accelerometer = Adafruit_MMA8451();


// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.



//Global vars:
int trigger_reading;
int last_trigger_reading;

// Use the Adafruit Sensor Event library to normalize reading sensor inputs:
  sensors_event_t sensor_event; 


uint8_t current_orientation= -99;
uint8_t last_orientation = -99;



void setup() {

// setup accelerometer:
    Serial.begin(9600);
  
  Serial.println("Adafruit MMA8451 test!");
    if (! accelerometer.begin()) {
    Serial.println("Couldnt start");
 //   while (1);
  }
  Serial.println("MMA8451 found!");
  accelerometer.setRange(MMA8451_RANGE_2_G);
//accelerometer.setRange(MMA8451_RANGE_4_G);
//accelerometer.setRange(MMA8451_RANGE_8_G);


  

  // Setup Neopixels:
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  // Setup 14-segment display:
   LED_14_seg.begin(LED_14_I2C_ADDR);  // pass in the address
   /*
   LED_14_seg.writeDigitRaw(3, 0x0);
  LED_14_seg.writeDigitRaw(0, 0xFFFF);
  LED_14_seg.writeDisplay();
  delay(200);
  LED_14_seg.writeDigitRaw(0, 0x0);
  LED_14_seg.writeDigitRaw(1, 0xFFFF);
  LED_14_seg.writeDisplay();
  delay(200);
  LED_14_seg.writeDigitRaw(1, 0x0);
  LED_14_seg.writeDigitRaw(2, 0xFFFF);
  LED_14_seg.writeDisplay();
  delay(200);
  LED_14_seg.writeDigitRaw(2, 0x0);
  LED_14_seg.writeDigitRaw(3, 0xFFFF);
  LED_14_seg.writeDisplay();
  delay(200);
  */
  
  LED_14_seg.clear();
  LED_14_seg.writeDisplay();
  /*
   // display every character, 
  for (uint8_t i='!'; i<='z'; i++) {
    LED_14_seg.writeDigitAscii(0, i);
    LED_14_seg.writeDigitAscii(1, i+1);
    LED_14_seg.writeDigitAscii(2, i+2);
    LED_14_seg.writeDigitAscii(3, i+3);
    LED_14_seg.writeDisplay();
    
    safe_delay(300);
  }
  */

  
}

void loop() {
  byte i;
  for (i=1; i <= 1000; i++) { // channel 0 is the console - start with 1

   safe_delay(300);
  }

  ServiceLights();
  ServiceSound();
  ServiceSensors();
  // Some example procedures showing how to display to the pixels:
  colorWipe(strip.Color(255, 0, 0), 50); // Red
  colorWipe(strip.Color(0, 255, 0), 50); // Green
  colorWipe(strip.Color(0, 0, 255), 50); // Blue
//colorWipe(strip.Color(0, 0, 0, 255), 50); // White RGBW
  // Send a theater pixel chase in...
  theaterChase(strip.Color(127, 127, 127), 50); // White
  theaterChase(strip.Color(127, 0, 0), 50); // Red
  theaterChase(strip.Color(0, 0, 127), 50); // Blue

  rainbow(20);
  rainbowCycle(20);
  theaterChaseRainbow(50);
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    safe_delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    safe_delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    safe_delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      safe_delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      safe_delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void ServiceSensors(){
/*
  Update global sensor values
*/



    accelerometer.getEvent(&sensor_event);

  /* Display the results (acceleration is measured in m/s^2) */
//  Serial.print("X: \t"); Serial.print(sensor_event.acceleration.x); Serial.print("\t");
//  Serial.print("Y: \t"); Serial.print(sensor_event.acceleration.y); Serial.print("\t");
//  Serial.print("Z: \t"); Serial.print(sensor_event.acceleration.z); Serial.print("\t");
  
   /* Get the orientation of the sensor */
  current_orientation = accelerometer.getOrientation();
/*  
  switch (o) {
    case MMA8451_PL_PUF: 
      Serial.println("Portrait Up Front");
      break;
    case MMA8451_PL_PUB: 
      Serial.println("Portrait Up Back");
      break;    
    case MMA8451_PL_PDF: 
      Serial.println("Portrait Down Front");
      break;
    case MMA8451_PL_PDB: 
      Serial.println("Portrait Down Back");
      break;
    case MMA8451_PL_LRF: 
      Serial.println("Landscape Right Front");
      break;
    case MMA8451_PL_LRB: 
      Serial.println("Landscape Right Back");
      break;
    case MMA8451_PL_LLF: 
      Serial.println("Landscape Left Front");
      break;
    case MMA8451_PL_LLB: 
      Serial.println("Landscape Left Back");
      break;
    }
  Serial.println();
  
  
  Serial.println("m/s^2 ");
  
 */ 

}

void ServiceLights(){


}
void ServiceSound(){


}
void safe_delay(unsigned long duration){
  unsigned long fake_delay_end = millis() + duration;
  while( millis() < fake_delay_end){

    ServiceLights();
    ServiceSound();
    ServiceSensors();
  }


}