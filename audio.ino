

void InitAudioFX(){
// Globals:
// ss -SoftwareSerial
// sfx - Adafruit_Soundboard
/*
  ss = SoftwareSerial(SFX_TX, SFX_RX);
  sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);
*/
  ss.begin(9600);
  delay(200); // give it a moment to warm up


    if (!sfx.reset()) {
      dprint("soundboard not found");
    } else {
      dprint("SFX board found");
      
    ListSFXFiles();
  sfx.playTrack(3);  
      
      
    }
  }

  
  
void AnimateAudioFX(unsigned long now){
  // globals
  // timer_audioFX = millis()+300;
  // acc
  acc = random(50);
  if (acc < 17){
    sfx.playTrack(acc);
  }
    timer_audioFX = now+4000; // check back in 4 seconds

}
  
  
// Assorted notes on the audio board:


// sfx.reset()
// uint8_t n = readnumber();
//   if (! sfx.playTrack(n) ) {
//
// or by name:
// char name[20];
// if (! sfx.playTrack(name) ) {
//
/*
uint16_t v;
if (! (v = sfx.volUp()) ) {
  dprintln("Failed to adjust");
} else {
  dprint("Volume: "); dprintln(v);
}
*/

// if (! (v=sfx.volDown()) ) {

