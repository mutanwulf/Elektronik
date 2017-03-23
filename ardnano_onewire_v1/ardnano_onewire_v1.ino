#include <OneWire.h>
 
OneWire ibutton (12); // iButton connected on PIN 42.
byte addr[8];
String keyStatus="";
int blue[8] = {1,206,207,231,1,0,0,152};
int master[8] = {1,208,151,1,1,0,0,59};
 
void setup(){
  Serial.begin(115200);
  Serial.println("Hello Pin12"); // print new line
  pinMode(10, OUTPUT);
}
 
void loop(){
  // Search for an iButton and assign the value to the buffer if found.
  digitalWrite(10, HIGH);   
  getKeyCode();
 // lcd.clear();
  if(keyStatus=="ok"){
      byte i;
      for( i = 0; i <8; i++) {
         //  lcd.print(":");
         //  lcd.print(addr[i], HEX);   
         Serial.print(i); 
         Serial.print(" --> ");
         Serial.println( addr[i], DEC);       
      }
        

      if(addr[1] == master[1] && addr[2] == master[2] && addr[3] == master[3] && addr[7] == master[7]){
       digitalWrite(10, LOW);
       delay(1500);
       digitalWrite(10, HIGH);
      } 
      else {
        blinkLED();
      } 
      Serial.println("Done.\n\n"); 
  }
  else if (keyStatus!="") {
    // lcd.print(keyStatus);
    Serial.println(keyStatus);
    }
   
  delay(1000);    
 
}

void blinkLED(){
  byte i;
  for( i = 0; i <5; i++) {
     digitalWrite(10, LOW);  
    delay(300);
    digitalWrite(10, HIGH); 
    delay(300);  
  }
}

void getKeyCode(){
  byte present = 0;
  byte data[12];
  keyStatus="";
  
  if ( !ibutton.search(addr)) {
      ibutton.reset_search();
      return;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
      keyStatus="CRC falsch";
      blinkLED();
      return;
  }
  
  if ( addr[0] != 0x01) {
      keyStatus="Kein iButton.";
      blinkLED();
      return;
  }
  keyStatus="ok";
  ibutton.reset();
}
