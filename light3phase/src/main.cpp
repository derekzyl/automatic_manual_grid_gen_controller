#include <Arduino.h>
#include <EEPROM.h>
#include <ArduinoJson.h>

// Define all the pins first
// LED pins
const int load_fail_led = 9;
const int manual_led = 2;
const int semi_auto_led = 3;
const int fully_auto_led = 4;
const int load_on_led = 5;
const int gen_on_led = 6;
const int gen_fail_led = 7;
const int grid_on_led = 8;
// Button pins
const int menu_button = 10;
const int select_button = 11;

// Define power sources availability
const int grid_check = A1;
const int generator_check = A2;
const int load_check = A3;

// Define relays
const int grid_relay = A6;
const int generator_relay = A5;
const int load_relay = 12;

// Bluetooth module is on Serial (TX/RX)

// Alarm pin
const int alarm_pin = A4;

// EEPROM address to store the mode
const int modeAddress = 0;
const int controlModeAddress = 1;

// Mode states
enum Mode { MANUAL, SEMI_AUTO, FULLY_AUTO };
enum ControlMode { GEN, GRID,STOP };

Mode currentMode;
ControlMode currentControlMode;


// string buffer
String receivedMessage = "";  // Buffer for incoming messages
bool messageComplete = false; //

// other  variables
boolean load_on = false;
boolean gen_on = false;
boolean grid_on = false;
 boolean load_fail = false;

// Function prototypes
void serialPrint(String message);
void selectMode(Mode mode);
void buttonPress();
void checkPowerSources();
void ledControl(int led, boolean state);
void manualMode();
void semiAutoMode();
void fullyAutoMode();
void saveModeToEEPROM(Mode mode);
Mode readModeFromEEPROM();
void saveControlModeToEEPROM(ControlMode mode);
ControlMode readControlModeFromEEPROM();
void controlMode(ControlMode mode);
void turnOffAllRelays();
void turnOffAllLEDs();
void turnOnAlarm();
boolean turnLoadOn();
boolean turnGenOn();
boolean turnGridOn();
void loadFailAction();
void sendLedData();
void receiveData();
void processMessage(String message);



/**
 * The setup function initializes various pins for LEDs, buttons, power source checks, relays, and an
 * alarm pin, and loads saved modes from EEPROM.
 */
void setup() {
  Serial.begin(9600);

  // Initialize LED pins
  pinMode(load_fail_led, OUTPUT);
  pinMode(manual_led, OUTPUT);
  pinMode(semi_auto_led, OUTPUT);
  pinMode(fully_auto_led, OUTPUT);
  pinMode(load_on_led, OUTPUT);
  pinMode(gen_on_led, OUTPUT);
  pinMode(gen_fail_led, OUTPUT);
  pinMode(grid_on_led, OUTPUT);

  // Initialize button pins
  pinMode(menu_button, INPUT);
  pinMode(select_button, INPUT);

  // Initialize power source check pins
  pinMode(grid_check, INPUT);
  pinMode(generator_check, INPUT);
  pinMode(load_check, INPUT);

  // Initialize relay pins
  pinMode(grid_relay, OUTPUT);
  pinMode(generator_relay, OUTPUT);
  pinMode(load_relay, OUTPUT);

  // Initialize alarm pin
  pinMode(alarm_pin, OUTPUT);

  // Load the saved mode from EEPROM
  currentMode = readModeFromEEPROM();
  currentControlMode = readControlModeFromEEPROM();
  selectMode(currentMode);
}

/**
 * The function `loop()` handles button presses, mode selection, power sources, LED updates, mode
 * execution, and Bluetooth commands in an Arduino sketch.
 */
void loop() {
  // Handle button press and mode selection
  buttonPress();

  // Check power sources and update LEDs
  checkPowerSources();
  sendLedData();
  receiveData();
    if (messageComplete) {
    processMessage(receivedMessage);
    receivedMessage = "";  // Clear the buffer after processing
    messageComplete = false; // Reset the flag
  }

  // Execute the current mode
  switch (currentMode) {
    case MANUAL:
      manualMode();
      break;
    case SEMI_AUTO:
      semiAutoMode();
      break;
    case FULLY_AUTO:
      fullyAutoMode();
      break;
  }

  // Check for Bluetooth commands
 
}

/**
 * The function serialPrint() in C++ prints a message to the serial monitor.
 * 
 * @param message The parameter "message" in the function "serialPrint" is a String type variable that
 * holds the message to be printed to the serial monitor.
 */
void serialPrint(String message) {
  Serial.println(message);
}

/**
 * The function `selectMode` sets the current mode, saves it to EEPROM, and then executes the
 * corresponding mode-specific functions based on the input mode.
 * 
 * @param mode The `mode` parameter in the `selectMode` function is an enum type `Mode`, which
 * represents different operating modes such as `MANUAL`, `SEMI_AUTO`, and `FULLY_AUTO`.
 */
void selectMode(Mode mode) {
  currentMode = mode;
  saveModeToEEPROM(mode);

  if (mode == MANUAL) {
    digitalWrite(manual_led, LOW);
      digitalWrite(semi_auto_led, HIGH);
    digitalWrite(fully_auto_led, HIGH);
   manualMode();
  } else if (mode == SEMI_AUTO) {
    digitalWrite(semi_auto_led, LOW);
        digitalWrite(manual_led, HIGH);

    digitalWrite(fully_auto_led, HIGH);
    semiAutoMode();
  } else if (mode == FULLY_AUTO) {
    digitalWrite(fully_auto_led, LOW);
        digitalWrite(manual_led, HIGH);
      digitalWrite(semi_auto_led, HIGH);

    fullyAutoMode();
  }
}


/**
 * The function `controlMode` sets the current control mode, saves it to EEPROM, and performs specific
 * actions based on the mode selected.
 * 
 * @param mode The `mode` parameter in the `controlMode` function is of type `ControlMode`, which is an
 * enum type representing different control modes. The possible values for `mode` are `GEN`, `GRID`,
 * and `STOP`.
 */
void controlMode(ControlMode mode) {
  currentControlMode = mode;
  saveControlModeToEEPROM(mode);

  if (mode == GEN) {
    turnGenOn();
  } else if (mode == GRID) {
    turnGridOn();
  } else if (mode == STOP) {
    turnOffAllRelays();
    turnOffAllLEDs();
  }

}


/**
 * The function checkPowerSources() reads the status of different power sources and updates
 * corresponding LEDs.
 */
void checkPowerSources() {
  digitalWrite(grid_on_led, digitalRead(grid_check));
  digitalWrite(gen_on_led, digitalRead(generator_check));
  digitalWrite(load_on_led, digitalRead(load_check));
}

/**
 * The function `ledControl` controls the state of an LED by setting it to either HIGH or LOW.
 * 
 * @param led The `led` parameter in the `ledControl` function is an integer that represents the pin
 * number of the LED that you want to control.
 * @param state The `state` parameter in the `ledControl` function is a boolean variable that
 * determines whether the LED should be turned on or off. If `state` is `true`, the LED will be turned
 * on (HIGH), and if `state` is `false`, the LED will be turned off
 */
void ledControl(int led, boolean state) {
  digitalWrite(led, state? HIGH : LOW);
}






// this is the various modes of the system

/**
 * The function `manualMode` sets the control mode to the current control mode.
 */
void manualMode() {

 ControlMode mode = currentControlMode;
 controlMode(mode);
}

/**
 * The semiAutoMode function checks if a select button is pressed to switch between manual and fully
 * automatic modes.
 */
void semiAutoMode() {

  if(digitalRead(select_button) == HIGH) {
   manualMode();
  }
  else{

fullyAutoMode();
  }
}


/**
 * The function `fullyAutoMode` checks if the grid is on, turns on the generator if needed, and then
 * turns on the load with appropriate actions for failures.
 */
void fullyAutoMode() {
boolean on_grid = turnGridOn();
if(on_grid == false) {
  boolean on_gen = turnGenOn();
  if(on_gen == true) {
  
    if(turnLoadOn() == false) {
 loadFailAction();
    }
   
}else{
  boolean turn_load_on = turnLoadOn();
  if(turn_load_on == false) {
    loadFailAction();
  }

}
}else{

  boolean turn_load_on = turnLoadOn();
  if(turn_load_on == false) {
    loadFailAction();
  }

}

  sendLedData();

}




// this is the various helper functions

/**
 * The function `turnOnAlarm` turns on an alarm by setting the alarm pin to HIGH for 10 seconds and
 * then turning it off.
 */
void turnOnAlarm() {
  digitalWrite(alarm_pin, HIGH);
  delay(5000);
  digitalWrite(alarm_pin, LOW);

  sendLedData();

}

/**
 * The function `turnLoadOn` attempts to turn on a load relay and checks for any failures during the
 * process.
 * 
 * @return The function `turnLoadOn()` returns a boolean value - either `true` or `false` based on the
 * conditions inside the function.
 */
boolean turnLoadOn() {

  digitalWrite(load_relay, HIGH);
  load_on = true;
  delay(10000);
  if(digitalRead(load_check) == LOW) {

loadFailAction( );
    return false;

  }
  else {
    digitalWrite(load_fail_led, HIGH);
    digitalWrite(load_relay, HIGH);
    load_on = true;
    load_fail = false;
    return true;
    }
  sendLedData();

}


/**
 * The function `turnGenOn` turns on a generator and checks for any failures.
 * 
 * @return The `turnGenOn()` function returns a boolean value. If the generator check is successful, it
 * returns `true`. If the generator check fails, it returns `false`.
 */
boolean turnGenOn() {
  digitalWrite(grid_relay, LOW);
    digitalWrite(load_relay, LOW);
    //led controls
  digitalWrite(grid_on_led, HIGH);

  digitalWrite(generator_relay, HIGH);
  gen_on = true;
    sendLedData();
  delay(5000);
  if(digitalRead(generator_check) == LOW) {
    digitalWrite(grid_relay, LOW);
    //led controls
    digitalWrite(gen_fail_led, HIGH);

     ledControl(gen_on_led, false);
    gen_on = false;
    turnOnAlarm();
    return false;
  } else {

    //led controls
    digitalWrite(gen_fail_led, HIGH);
    ledControl(gen_on_led, false);
    return true;
  }
  sendLedData();

}

/**
 * The function `turnGridOn` attempts to turn on the grid by activating relays and checking for
 * successful grid connection.
 * 
 * @return The function `turnGridOn()` will return a boolean value. If the grid check is LOW, it will
 * return `false`. Otherwise, it will return `true`.
 */
boolean turnGridOn() {
  digitalWrite(generator_relay, LOW);
 digitalWrite(load_relay, LOW);
  digitalWrite(grid_relay, HIGH);

  //led control
    digitalWrite(grid_on_led, LOW);
  ledControl(gen_on_led, true);
  gen_on = false;
  sendLedData();
  delay(5000);
  if(digitalRead(grid_check) == LOW) {
    
    digitalWrite(grid_relay, LOW);
    // digitalWrite(load_relay, LOW);
ledControl(grid_on_led, false);


    grid_on = false;

    return false;
  } else {
    //led control
    digitalWrite(grid_on_led, LOW);
    return true;
  }
  sendLedData();

}

/**
 * The function `loadFailAction` checks for load failure and takes appropriate actions if a failure is
 * detected.
 */
void  loadFailAction(){
  if(load_fail == true || digitalRead(load_check) == LOW){
    digitalWrite(load_relay, LOW);
    ledControl(load_fail, true);
    ledControl(load_on, false);
    //led control
    ledControl(load_fail_led, false);
    ledControl(load_on_led, true);
     load_on = false;
    load_fail = true;

       
    digitalWrite(grid_relay,LOW);
    digitalWrite(generator_relay,LOW);
    turnOnAlarm();
    
  }
  sendLedData();


}

/**
 * The function `buttonPress` handles button debouncing and toggles between different modes based on
 * button presses.
 */
void buttonPress() {
  static unsigned long lastDebounceTime = 0;
  const unsigned long debounceDelay = 50;
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (digitalRead(menu_button) == HIGH) {
      if (currentMode == MANUAL) {
        selectMode(SEMI_AUTO);
      } else if (currentMode == SEMI_AUTO) {
        selectMode(FULLY_AUTO);
      } else {
        selectMode(MANUAL);
      }
      lastDebounceTime = millis();
    }
    if(digitalRead(select_button) == HIGH) {
      if(currentControlMode == GEN) {
        controlMode(GRID);
      } else if(currentControlMode == GRID) {
        controlMode(STOP);
      } else {
        controlMode(GEN);
      }
      lastDebounceTime = millis();
    }
  }
}


//handling eeprom data
/**
 * The function saves a ControlMode value to EEPROM memory in C++.
 * 
 * @param mode The `mode` parameter represents the control mode that you want to save to the EEPROM. It
 * is of type `ControlMode`, which is likely an enum or a custom data type that defines different
 * control modes for your system.
 */
void saveControlModeToEEPROM(ControlMode mode) {
  EEPROM.write(controlModeAddress, mode);
}
ControlMode readControlModeFromEEPROM() {
  return static_cast<ControlMode>(EEPROM.read(controlModeAddress));
}

/**
 * The function saves a Mode variable to EEPROM memory in C++.
 * 
 * @param mode The `mode` parameter is the mode that you want to save to the EEPROM.
 */
void saveModeToEEPROM(Mode mode) {
  EEPROM.write(modeAddress, mode);
}



/**
 * The function reads a mode value from EEPROM and returns it as an enum Mode type.
 * 
 * @return The function `readModeFromEEPROM()` is returning a value of type `Mode`, which is obtained
 * by reading the data stored in the EEPROM at the `modeAddress` and then casting it to the `Mode` enum
 * using `static_cast`.
 */
Mode readModeFromEEPROM() {
  return static_cast<Mode>(EEPROM.read(modeAddress));
}




/**
 * The function `turnOffAllRelays` turns off all relays and corresponding LEDs.
 */
void turnOffAllRelays() {
  digitalWrite(grid_relay, LOW);
  digitalWrite(generator_relay, LOW);
  digitalWrite(load_relay, LOW);
  // indicate with the leds as well that the relays are off
  digitalWrite(grid_on_led, LOW);
  digitalWrite(gen_on_led, LOW);
  digitalWrite(load_on_led, LOW);

  delay(1000);

  sendLedData();
}

/**
 * The function `turnOffAllLEDs` turns off all LEDs by setting their output to LOW.
 */
void turnOffAllLEDs() {
  digitalWrite(load_fail_led, HIGH);
  digitalWrite(manual_led, HIGH);
  digitalWrite(semi_auto_led, HIGH);
  digitalWrite(fully_auto_led, HIGH);
  digitalWrite(load_on_led, HIGH);
  digitalWrite(gen_on_led, HIGH);
  digitalWrite(gen_fail_led, HIGH);
  digitalWrite(grid_on_led, HIGH);
    delay(1000);
  sendLedData();

}






// this function is responsible for getting the status of the pins

boolean getPinStatus(int pin) {
  return digitalRead(pin) == HIGH? true : false;
}
/**
 * The function setPinStatus sets the status of a specified pin to either HIGH or LOW.
 * 
 * @param pin The `pin` parameter is an integer that represents the pin number on the microcontroller
 * or development board where you want to set the status (HIGH or LOW).
 * @param status The `status` parameter is a boolean value that determines whether the pin should be
 * set to HIGH or LOW. If `status` is true, the pin will be set to HIGH, and if `status` is false, the
 * pin will be set to LOW.
 */
void setPinStatus(int pin, boolean status) {
  digitalWrite(pin, status? HIGH : LOW);
}




// lets handle the bluetooth communication for sending led data to the app
/**
 * The function sends LED status data in JSON format over a serial connection.
 */
void sendLedData() {
  // Serial.print("start_json");
   JsonDocument  jsonDoc;
  jsonDoc["load_fail"] = getPinStatus(load_fail_led)?false:true;
  jsonDoc["manual"] = getPinStatus(manual_led)?false:true;
  jsonDoc["semi_auto"] = getPinStatus(semi_auto_led)?false:true;
  jsonDoc["fully_auto"] = getPinStatus(fully_auto_led)?false:true;
  jsonDoc["load_on"] = getPinStatus(load_on_led)?false:true;
  jsonDoc["gen_on"] = getPinStatus(gen_on_led)?false:true;
  jsonDoc["gen_fail"] = getPinStatus(gen_fail_led)?false:true;
  jsonDoc["grid_on"] = getPinStatus(grid_on_led)?false:true;

  


  // serializeJsonPretty(jsonDoc, Serial);+
+  // char jsonString[256]; // Buffer to hold JSON as a string
  // serializeJson(jsonDocument, jsonString); // Serialize JSON to string
serializeJson(jsonDoc, Serial);


// Serial.println(jsonString); // Send JSON string over Bluetooth

  delay(1000);


}



void receiveData() {
  while (Serial.available() > 0) {
    char receivedChar = Serial.read();
    
    if (receivedChar == '\n') {  // Assuming messages end with a newline character
      messageComplete = true;
      break;
    } else if (receivedChar != '\r') { // Ignore carriage return character
      receivedMessage += receivedChar;  // Append character to message buffer
    }
  }
}


void processMessage(String message) {


  if (message == "man") {
    Serial.print("manual");
    selectMode(MANUAL);
  } else if (message == "semi") {
    Serial.print("semi");
    selectMode(SEMI_AUTO);
  } else if (message == "auto") {
    Serial.print("auto");
    selectMode(FULLY_AUTO);
  } else if (message == "gen") {
    if (currentMode == MANUAL || currentMode == SEMI_AUTO) {
      Serial.print("gen");
      controlMode(GEN);
    }
  } else if (message == "grid") {
    if (currentMode == MANUAL || currentMode == SEMI_AUTO) {
      Serial.print("grid");
      controlMode(GRID);
    }
  } else if (message == "stop") {
    if (currentMode == MANUAL || currentMode == SEMI_AUTO) {
      Serial.print("stop");
      controlMode(STOP);
    }
  } else {
    Serial.println("Unknown command");
  }
}

// // lets handle the bluetooth communication for receiving data from the app

// /**
//  * The function `receiveData` reads a command from the serial input and selects a mode based on the
//  * received command.
//  */
// void receiveData() {
//  if (Serial.available() > 0) {
//   Serial.print("inside bluetooth");
//   // the commands are: 
//   const  char command = Serial.read();

//   String message = String(command);
//   Serial.print(message);
// switch (command)
// {
// case 'manual':{
//   Serial.print("manual");
//   selectMode(MANUAL);
//   break;}
// case 'semi':
//   selectMode(SEMI_AUTO);
//   break;


// case 'auto':
//   selectMode(FULLY_AUTO);

//   break;
//   case 'gen':{
//  if(currentMode == MANUAL || currentMode == SEMI_AUTO )  controlMode(GEN);
//   break;}
//   case 'grid':{
//   if(currentMode == MANUAL || currentMode == SEMI_AUTO ) controlMode(GRID);
//   break;}
//   case 'stop':
//   {
//     if(currentMode == MANUAL || currentMode == SEMI_AUTO )  controlMode(STOP);
//   break;
// }
// default:
//   break;
// }



//   }}

