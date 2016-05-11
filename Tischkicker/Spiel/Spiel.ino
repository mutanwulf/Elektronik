/*
  LiquidCrystal Library - display() and noDisplay()

 The circuit:
 * LCD RS pin to digital pin 10
 * LCD Enable pin to digital pin 9
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

 http://www.arduino.cc/en/Tutorial/LiquidCrystalDisplay

 */
#include <LiquidCrystal.h>

// Setup the LiquidCrystal library with the pin numbers we have
// physically connected the module to.
LiquidCrystal lcd(10, 9, 5, 4, 3, 2);

const int Spieler1 = 6;
const int Spieler2 = 7;
const int Taster3 = 8;

int reading;               // Wert des Eingangs
int previous = HIGH;    // Vorheiger Wert des Eingangs
int pressed1 = HIGH;
int pressed2 = HIGH;

int AltesSpiel = 0;
// folgende Variablen müssen vom Typo unsigned long sein da millis() disen Variablentyp liefert.

unsigned long time = 0;         // letzter Zeitwert bei dem der Ausgangzustand wechselte.
unsigned long debounce = 200;   // Entprellzeit

// Spiel: 0 = steht; 1 = läuft
boolean Spiel = false;
int Tore1 = 0;
int Tore2 = 0;
/* Spielmodus
 *  0 = Info anzeigen
 *  1 = normal zählen ohne Stop
 *  2 = bis 10 Spiel
 *  3 = 10 Minuten Spiel
 *  4 = Spielauswahl
 */
int Spielmodus = 0;
int Auswahl = 0;
boolean displayed = false;

int lastUpdate = 0;  
int currentIndex = 0;  
char* lyrics[] = {  
  " Hallo Spieler, ", 
  " waehlt aus mit ",
  "Taste3 zwischen ",
  "  Normalspiel,  ",
  "  bis 10 Tore   ",
  "   Zeitspiel    "};
char* Spiele[] = {
  " normales Spiel ", 
  " bis 10 spielen ",
  "   Zeitspiel    "};
  String Roter = "";
  String Gruener = "";
 char* Rote[] = {
    "FCB",
    "VFB",
    "M05",
    "FCA",
    "FCK",
    "B04",
    "F95",
    "FCN",
    "FCK",
    "FCI",
    };
 char* Gruene[] = {
    "WOB",
    "BMG",
    "H96",
    "BRE",
    "SGF",
    "S04",
    "STP",
    "DSC",
    "EBS",
    "HSV",
    };

void setup() {  
  // Setup the number of columns and rows that are available on the LCD. 
  randomSeed(analogRead(0));
  lcd.begin(16, 2);
  pinMode(Spieler1, INPUT);
  digitalWrite(Spieler1, HIGH);      // Pull Up Widerstand aktivieren
  pinMode(Spieler2, INPUT);
  digitalWrite(Spieler2, HIGH);      // Pull Up Widerstand aktivieren
  pinMode(Taster3, INPUT);
  digitalWrite(Taster3, HIGH);      // Pull Up Widerstand aktivieren
 // attachInterrupt(4, interruptRoutine, LOW);
 Roter = Rote[random(0,9)];
 Gruener = Gruene[random(0,9)];
}

void loop() {  
    reading = digitalRead(Taster3);
   // Beim Wechsel des Eingangs von LOW zu HIGH und nach der Entprellzeit:
   /*
  if (reading == HIGH && previous == LOW && millis() - time > debounce) {
    time = millis();   
    Tue irgendwas
    }
*/
  // Beim Wechsel des Eingangs von HIGH zu LOW und nach der Entprellzeit:
  if (reading == LOW && previous == HIGH && millis() - time > debounce) {
    time = millis();  
    
    if (Spielmodus == 4) {
      Auswahl++;
      if (Auswahl == 3)  {
        Auswahl = 0;
      }
    } else if (Spielmodus == 0) {
      Spielmodus = 4;
      lcd.clear();
      
    } else {
  // Hier nun RESET
  if (Spielmodus <= 5){
     AltesSpiel = Spielmodus;
     lcd.clear();
    } 
    Spielmodus = 5;  
    }
   }
  
  previous = reading;

  switch(Spielmodus) {
    case 0:
    proginfo();
    break;
    case 1:
    prog1();
    break;
    case 2:
    prog2();
    break;
    case 3:
    prog3();
    break;
    case 4:
    Spielauswahl();
    break;
    case 5:
    Reset();
    break;
    default:
    Spielmodus = 0;
    break;
  }
}

//Reset
void Reset(){
      lcd.setCursor(0, 0);
      lcd.print(" Spiel resetten ");
      lcd.setCursor(0, 1);
      lcd.print("  Spieler2  Ja  ");
       reading = digitalRead(Spieler2);
    if (reading == LOW && pressed2 == HIGH && millis() - time > debounce) {
      time = millis();  
      Tore2 = 0;
      Tore1 = 0;   
      Spielmodus = 0;
      asm volatile ("  jmp 0");     
    }
    pressed2 = reading;
    reading = digitalRead(Spieler1);
    if (reading == LOW && pressed1 == HIGH && millis() - time > debounce) {
      time = millis();  
      lcd.clear();
      Spielmodus = AltesSpiel;  
    }
    pressed1 = reading;
  
  }
// normales Spiel
void prog1(){
  // lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   " + Roter + "    " + Gruener + "  ");
      lcd.setCursor(0, 1);
      lcd.print("    " + String(Tore1) + "  :   " + String(Tore2));
              reading = digitalRead(Spieler1);
    if (reading == LOW && pressed1 == HIGH && millis() - time > debounce) {
      time = millis();  
      Tore1++;  
    }
    pressed1 = reading;
    reading = digitalRead(Spieler2);
    if (reading == LOW && pressed2 == HIGH && millis() - time > debounce) {
      time = millis();  
      Tore2 ++;    
    }
    pressed2 = reading;
}

// bis 10
void prog2(){
   
      lcd.setCursor(0, 0);
      lcd.print("   bis 10    ");
      lcd.setCursor(0, 1);
      lcd.print(String(Spielmodus) + "   ");
}

//10 Minuten Spiel
void prog3(){
  
      lcd.setCursor(0, 0);
      lcd.print("   Zeitspiel   ");
      lcd.setCursor(0, 1);
      lcd.print(String(Spielmodus) + "   ");
}

void proginfo(){
  int jetzt = millis();
  if ((jetzt  - lastUpdate) >= 1600)
  {
    // Move the cursor back to the first column of the first row.
    lcd.setCursor(0, 0);
    if (currentIndex == 0 || currentIndex == 2 || currentIndex == 4)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(lyrics[currentIndex]);
    }
    else
    {
      lcd.setCursor(0, 1);
      lcd.print(lyrics[currentIndex]);
    }
    // Increment or reset the current index.
    if (currentIndex == 5) {
      currentIndex = 0;
    } else {
      currentIndex += 1;
    }
    lastUpdate = jetzt;
  } 
}

void Spielauswahl(){
        reading = digitalRead(Spieler1);
    if (reading == LOW && pressed1 == HIGH && millis() - time > debounce) {
      time = millis();  
      Spielmodus = Auswahl + 1;  
      Spiel = true;     
    }
    pressed1 = reading;
    
   if (Spiel == false) {      
      lcd.setCursor(0, 0);
      lcd.print("  Spielauswahl  ");
      lcd.setCursor(0, 1);
      lcd.print(Spiele[Auswahl]);
      displayed = true;
  } else {     
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("   Spielwahl    ");
      lcd.setCursor(0, 1);
      lcd.print(String(Spielmodus) + "   ");
      delay(1000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Spiel startet in");
      for (int i=0; i < 5; i++){         
         lcd.setCursor(0, 1);
         lcd.print("   " + String(5-i) + " Sekunden   ");
         delay(1000);
       }
       lcd.clear();
     } 
}

  void Start(){
    
    }

