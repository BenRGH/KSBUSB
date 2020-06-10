#include <Keyboard.h>

// Presses keys based on switch and button activation
// all inputs should be debounced!

// Switches
struct sCONTROL {
  // switch interface
  bool status;  // if it's on
  byte pin;     // digital for switches
  byte key;     // keyboard key
};
typedef struct sCONTROL sCONTROLRecord;

// ASCII keys here: http://www.asciitable.com/
// https://wiki.kerbalspaceprogram.com/wiki/Key_bindings
const int numberOfSwitches = 6;
sCONTROLRecord sCONTROLS[numberOfSwitches] = {
  {false, 2, 0},   // Launch Lock - NO KEY, FALSE PREVENTS STAGE
  {false, 3, 115}, // SAS
  {false, 4, 114}, // RCS
  {false, 5, 98},  // Breaks
  {false, 6, 103}, // Gear
  {false, 7, 108}, // Lights
};

// Buttons
struct bCONTROL {
  // button interface
  bool status;        // if it's being pressed
  short lowerTrigger; // lower value for button trigger, e.g. 100 - 500 is the Stage button
  short upperTrigger; // upper value for button trigger
  byte key;           // keyboard key
};
typedef struct bCONTROL bCONTROLRecord;

#define buttonPin A0
// !    change trigger ranges   !
byte numberOfButtons = 6;
bCONTROLRecord bCONTROLS[6] = {
  {false, 14, 16, 32}, // Stage
  {false, 19, 21, 49}, // Action group 1
  {false, 22, 24, 50}, // Action group 2
  {false, 27, 30, 51}, // Action group 3
  {false, 33, 36, 52}, // Action group 4
  {false, 38, 42, 8},  // Abort
};

void setup() {
  // Button pin
  // pullup resistor so it goes high unless connected to ground, CHECK IT CAN HANDLE ALL THESE !!!
  pinMode(buttonPin, INPUT_PULLUP); // for buttons
  
  // Switch pins
  for(byte i = 0; i < numberOfSwitches; i++){
    pinMode(sCONTROLS[i].pin, INPUT_PULLUP);
  }

  // Interfacing
  Serial.begin(9600); // debug
  Keyboard.begin();
}

void loop() {
  delay(100); // stops accidental multi printing due to small voltage dips
  
  // check all switches
  for(byte i = 0; i < numberOfSwitches; i++){
    sCheckControl(i, sCONTROLS[i]);
  }

  // check all buttons
  short pinVal = analogRead(buttonPin);
  for(byte i = 0; i < numberOfButtons; i++){
    bCheckControl(i, pinVal, bCONTROLS[i]);
    // anytime the value is outside all of the ranges it will be ignored
  }
}

void sCheckControl(byte index, sCONTROL thisSwitch){
  // checks value of pin and compares to status, sends key
  bool switchVal = digitalRead(thisSwitch.pin) == false; // input is opposite so we correct here
    
//  Serial.print(thisSwitch.pin); // debug
//  Serial.print(" ");
//  Serial.print(thisSwitch.status);
//  Serial.print(" :");
//  Serial.println(switchVal);
  
  if(switchVal == true && thisSwitch.status == false){ // activated and we don't already know
    Keyboard.write(thisSwitch.key); // press key
    sCONTROLS[index].status = true; // change status of switch to active
    
    Serial.print("switch on: "); // debug
    Serial.println(thisSwitch.key);
    
    if(thisSwitch.key == 98) {
      // for toggle breaks to work needs the mod https://github.com/severedsolo/DoubleTapBrakes/releases
      // requires pressing b again
      Keyboard.write(thisSwitch.key);
    }
    
  } else if (switchVal == false && thisSwitch.status == true){ // deactivated and we don't already know
    sCONTROLS[index].status = false; // has been disactivated
    Keyboard.write(thisSwitch.key); // press key
    
    Serial.print("switch off: "); // debug
    Serial.println(thisSwitch.key);

    if(thisSwitch.key == 98) {
      // for toggle breaks to work needs the mod https://github.com/severedsolo/DoubleTapBrakes/releases
      // requires pressing b again
      Keyboard.write(thisSwitch.key);
    }
  }
}

void bCheckControl(byte index, short pinVal, bCONTROL thisButton){
  // checks status of button and updates if necessary
  
  if(pinVal >= thisButton.lowerTrigger && pinVal <= thisButton.upperTrigger){
    // it's this button

    if(thisButton.status != true && (thisButton.key != 32 || sCONTROLS[0].status == true)){
      // activate and send keypress when this button isn't known as being pressed
      // not allowed to stage while LL is false
      
      bCONTROLS[index].status = true;
      
      Serial.println(thisButton.key); // debug
      Serial.println(": button pressed");
      
      Keyboard.write(thisButton.key);  
    }
    
  } else {
    // it's not this button so disactivate
    bCONTROLS[index].status = false;
  }
}
