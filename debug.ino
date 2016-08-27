// debugging functions:
void ListSFXFiles(){
  uint8_t files = sfx.listFiles();

  Serial.println(F("File Listing"));
  Serial.println(F("========================"));
  Serial.println();
  Serial.print(F("Found ")); Serial.print(files); Serial.println(F(" Files"));
  for (uint8_t f=0; f<files; f++) {
    Serial.print(f);
    Serial.print(F("\tname: ")); Serial.print(sfx.fileName(f));
    Serial.print(F("\tsize: ")); Serial.println(sfx.fileSize(f));
  }
  Serial.println(F("========================"));
  
}
