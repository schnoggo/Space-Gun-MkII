

void InitAudioFX(){
// Globals:
// ss -SoftwareSerial
// sfx - Adafruit_Soundboard
/*
  ss = SoftwareSerial(SFX_TX, SFX_RX);
  sfx = Adafruit_Soundboard(&ss, NULL, SFX_RST);
*/
pinMode(AUDIO_SENSE, INPUT);
  ss.begin(9600);
  delay(200); // give it a moment to warm up


    if (!sfx.reset()) {
      dprint(F("soundboard not found"));
    } else {
      dprint(F("SFX board found"));

    ListSFXFiles();
  sfx.playTrack( (uint8_t)0);

  while (IsFXPlaying()){
      delay(20);
  }

    }

  }

boolean IsFXPlaying(){
  boolean retVal = true;
  if (digitalRead(AUDIO_SENSE)){
    retVal = false;
  }
  return retVal;
}
void StartAudioFX(byte anim_num){

  sfx.playTrack(anim_num);

}

void InteruptAudioFX(byte anim_num){
  if (IsFXPlaying()){
    sfx.stop();

  }
  StartAudioFX(anim_num);


}


void StopAudioFX(){
  //   sfx.stop();

    }


void AnimateAudioFX(unsigned long now){
  // globals
  // timer_audioFX = millis()+300;
  // acc
  /*
  acc = random(10);
  if (acc < 3){
    sfx.playTrack(acc +1 );
  }
  */
    timer_audioFX = now+40;
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
  dprintln(F("Failed to adjust"));
} else {
  dprint(F("Volume: ")); dprintln(v);
}
*/

// if (! (v=sfx.volDown()) ) {
