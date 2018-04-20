/* 
 *  ESP Aqua Light Control
 * 
 *  controls with PWM the lightbars
 *  thx to ZAK for help
*/

#define STATION

const char* WiFiHostname = "esp8266Aqua";      // Host name on the network
const char* mdnsName = "esp8266Aqua";          // Domain name for the mDNS responder

#include "Networkinit.hpp"
#include "WebServer.hpp"
#include <PubSubClient.h>

// PIN SETTINGS
// rgbwc 13,12,14,5,4
const byte switchPin = 5;

// OTHER SETTINGS
byte state = 0; // 0=normal, 1 = fadeup; 2 = fadedown; 3 = allOff

long fadeTime = 20000; // 20s for fadedif
int iFadeStep = 5; // up/down this stepvalue
long lastFadeTime = 0; //last fading millis
int fadeCount = 0;
int iResolution = 100; //100=%, normal ESP=1023

String sketchversion = "ESP_AquaControl-v0.18-0420c";

typedef struct {
	const byte Pin;
	int Intens;
	int IntensMax;
	boolean FadeDone;
	int FadeDownStart;
	int FadeUpStart;
} LightBar;

LightBar LBWarm = {12, 0, 75, true, 8, 2}; 	// Pin 12
LightBar LBCold = {13, 0, 75, true, 10, 0}; // Pin 13
LightBar LBGlow = {14, 0, 95, true, 0, 8}; 	// Pin 14

// START PROGRAM

WiFiClient espClient;

void setBar(LightBar& LBBar, boolean mode){
  pinMode(LBBar.Pin, mode);
  writeBar(LBBar);
}

void writeBar(LightBar& LBBar){
  analogWrite(LBBar.Pin,LBBar.Intens);
}

void setup(void) {
  analogWriteRange(iResolution);
  pinMode(switchPin, INPUT_PULLUP);
    
  setBar(LBGlow, OUTPUT);
  setBar(LBWarm, OUTPUT);
  setBar(LBCold, OUTPUT);

  allOff();
 
  Serial.begin(115200);
  Serial.println("");

  startWiFi(); 

  printIP();

  startWebServer();
  
  lastFadeTime = millis();
}

void loop(void){
  switch (state) {
    case 1: 
    case 2:
      if (millis() - lastFadeTime > fadeTime){
        fadeBar(LBGlow, state);
        fadeBar(LBWarm, state);
        fadeBar(LBCold, state);
        lastFadeTime = millis();
        fadeCount++;
      } 
      fadeCheck();
      break;
    case 3: 
      allOff();
      break;  
  }
    
  // handle http:
  server.handleClient();
}

void startWebServer(){
  server.on("/", []() {
    // server args for state
    if (server.arg("s") != "") {
      if (server.arg("s") == "1" || server.arg("s") == "2" || server.arg("s") == "3") {
         updateState(server.arg("s").toInt());
      } else {
       updateState(0);
      }
    }
    
    if (server.arg("g") != "") {
     updateBar(LBGlow, (int) server.arg("g").toInt());
    }
    if (server.arg("c") != "") {
      updateBar(LBCold, (int) server.arg("c").toInt());
    }
    if (server.arg("w") != "") {
      updateBar(LBWarm, (int) server.arg("w").toInt());
    }
	if (server.arg("p") != "") {
      updateStep((int) server.arg("p").toInt());
    }
    if (server.arg("t") != "") {
      updateTime((int) server.arg("t").toInt());
    }
        
    // json-like output
    String s = "{\n   \"s\":";
    s += state;
    s += ",\n   \"g\":";
    s += LBGlow.Intens;
    s += ",\n   \"c\":";
    s += LBCold.Intens;
    s += ",\n   \"w\":";
    s += LBWarm.Intens;
    s += ",\n   \"t\":";
    s += fadeTime;
    s += "\n}";
    
    server.send(200, "text/plain", s);
    
  });
  server.on("/status", []() {

    String st = "Status: ";
    st += char(ESP.getChipId()) + "\n <br>";
    st += "sketchversion: " + sketchversion + "<br>";
    st += "Signal: " + String(rssi) + " dBm<br>";
    server.send(200, "text/html", st);
  });

  server.on("/webupdate", []() {
    server.send(200, "text/html", updateHTTP);
  });
  
  server.onFileUpload([]() {
    if(server.uri() != "/update") return;
    detachInterrupt(switchPin);
    allOff();
   
    HTTPUpload& upload = server.upload();
   
    if(upload.status == UPLOAD_FILE_START){
      state = 0;
      //Serial.setDebugOutput(true);
      Serial.printf("Update: %s\n", upload.filename.c_str());
      uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
      if(!Update.begin(maxSketchSpace)){//start with max available size
        Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_WRITE){
      if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
        Update.printError(Serial);
      }
    } else if(upload.status == UPLOAD_FILE_END){
      if(Update.end(true)){ //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
      //Serial.setDebugOutput(false);
    }
    yield();
  });
  
  server.on("/update", HTTP_POST, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/plain", (Update.hasError())?"UPDATE FAIL":"UPDATE SUCCESS");
    ESP.restart();
  });
  
  server.begin();
  Serial.println("HTTP server: Started");
 }

void updateStep(int newValue) {
  if (newValue <= 0) {
    newValue = iResolution / 10;
  } else if(newValue > (iResolution / 2)) {
    newValue = iResolution / 2;
  } 
  iFadeStep = newValue;
}

void updateTime(int newValue) {
  if (newValue <= 0) {
    newValue = 500;
  } else if(newValue > 600000) {
    newValue = 10000;
  } 
  fadeTime = newValue;
}

void updateVersion() {
    String payload = String(sketchversion);
}

bool updateBar(LightBar& LBBar, int newValue){
  bool ret = false;
  if (newValue <= 0) {
    newValue = 0;
     ret = true;
  } else if(newValue > LBBar.IntensMax) {
    newValue = LBBar.IntensMax;
     ret = true;
  } 
   LBBar.Intens = newValue;
   writeBar(LBBar);
   return ret;
}

void allOff(){
  updateBar(LBGlow,0);
  updateBar(LBWarm,0);
  updateBar(LBCold,0);
}

void fadeBar(LightBar& LBBar, int fadeDirection){
  if (!LBBar.FadeDone) {
    switch (fadeDirection){
      case 1:
        if (fadeCount >= LBBar.FadeUpStart)  {
            LBBar.FadeDone = updateBar(LBBar, LBBar.Intens + iFadeStep);
        };
        break;
      case 2:
        if (fadeCount >= LBBar.FadeDownStart)  {
             LBBar.FadeDone = updateBar(LBBar, LBBar.Intens - iFadeStep);
        };
        break;
    }
  }
}
  
void updateState(int newState) {
  bool bVal = true;
  if (newState != state) {  // if state changed, set values, otherwise do nothing
   if (state != 0) {
    bVal = false;
   }
   LBGlow.FadeDone = bVal;
   LBWarm.FadeDone = bVal;
   LBCold.FadeDone = bVal;
   fadeCount = 0;
   state = newState;  
  }
}

// check fading finish
void fadeCheck(){
  if (LBGlow.FadeDone && LBWarm.FadeDone && LBCold.FadeDone){
      updateState(0);
  }
  
  // *2 damit die doppelte Anzahl ausgeführt werden kann --> noch mal nachdenken
  if (fadeCount > ((iResolution / iFadeStep)) * 2) {
    updateState(0);
  }
}
