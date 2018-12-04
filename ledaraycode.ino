/* 
 * Christina Cuneo
 * adapted from initial provided code
 * LED Array 
 * ENGR 40M lab 3B
 * Stanford University
 * July/August 2018
*/
const byte ANODE_PINS[8] = {6, 7, 8, 9, 10, 11, 12, 13}; //sets anode pins
const byte CATHODE_PINS[8] = {A3, A2, A1, A0, 2, 3, 4, 5}; //sets cathode pins
int brightness = 14;
const int FLEX_PIN = A4; // Pin connected to voltage divider output
const int FLEX_PIN2 = A5; //second pin connected to second voltage divider //MORE TODO
//const  int  TIME_DELAY = 500;
int iterator = 0; //this allows two alternate between row-moving and col-moving and have them act independently based on each flex sensor

// Measure the voltage at 5V and the actual resistance of your
// 47k resistor, and enter them below:
const float VCC = 4.98; // Measured voltage of Ardunio 5V line
const float R_DIV = 10000.0; //measured value of resistor using

// Upload the code, then try to adjust these values to more
// accurately calculate bend degree.
const float STRAIGHT_RESISTANCE = 30000.0; // resistance when straight for 2.5in
const float BEND_RESISTANCE = 180000.0; // 2.5 inch when bent
int flexMax = 0;
int flexMin = 1022;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(FLEX_PIN, INPUT); //sets first flex resistor pin as input
  pinMode(FLEX_PIN2, INPUT); //sets secodn flex resistor pin as input


  for (int i = 0; i < 8; i++) {
    pinMode(ANODE_PINS[i], OUTPUT); //sets each anode pin as an output
    pinMode(CATHODE_PINS[i], OUTPUT); //sets each anode pin as an ouput
    digitalWrite(ANODE_PINS[i], HIGH); //Should set them all to high which means they're off
    digitalWrite(CATHODE_PINS[i], HIGH); //Should set them all to high which would be off

  }

  //  Serial.begin(115200);
  Serial.setTimeout(100);


}

/* Function: display
   -----------------
   Runs through one multiplexing cycle of the LEDs, controlling which LEDs are
   on.
*/

void display(byte pattern[8][8]) {
  for (int i = 0; i < 8; i++) { //   for each anode (+/row) wire
    for (int j = 0; j < 8; j++) {//     for each cathode (-/column) wire
      if (pattern[i][j] == 0) { //       look up in pattern whether this LED should be on or off
        digitalWrite(CATHODE_PINS[j], LOW); //simple turn on
      } else {
        digitalWrite(CATHODE_PINS[j], HIGH); //should not be on
      }
    }//end for
    digitalWrite(ANODE_PINS[i], LOW); //activate anode (+) wire
    delayMicroseconds(800); //wait a short time
    digitalWrite(ANODE_PINS[i], HIGH); //deactivate anode (+) wire
  }//end for
}


//this is just the general move pattern that moves both down and over
void  movePattern(byte  pattern [8][8]) { 
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      if (i < 7) {
        pattern[i][j] = pattern[i + 1][j - 1];
      } else {
        //  pattern[i][j]=pattern[i][0];
      }
    }
  }
  chooseRandomInTopRow(pattern); //maybe uncomment this?
}

//this moves the pattern down one row and then randomizes the top row
void  movePatternDown(byte  pattern [8][8]) {
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      if (i < 7) {
        pattern[i][j] = pattern[i + 1][j]; //loop starts at bottom and replicates the row above
      } else {
        // pattern[i][j]=pattern[i][0];
      }
    }
  }
  chooseRandomInTopRow(pattern); //new top row
}

//this moves the pattern over one column and then randomizes the leftmost cpl
void  movePatternOver(byte  pattern [8][8]) {
  for (byte i = 0; i < 8; i++) {
    for (byte j = 0; j < 8; j++) {
      // if (i<7){
      pattern[i][j] = pattern[i][j + 1]; //starts with rightmost col and duplicates col to left
      //}else{
      //       pattern[i][j]=pattern[i][0];
      //}
    }
  }
  chooseRandomOnLeftSide(pattern); //new leftmost col
}


//this randomizes a row
void  chooseRandomInTopRow(byte  pattern [8][8]) {
  for (int i = 0; i < 8; i++) { //to go through each LED of a row
    if (random() % 6 == 0) { //each LED has a one-in-six chance of being lit, which seems to look the nicest
      pattern[7][i] = 0; //if you reach here the LED is on
    } else {
      pattern[7][i] = 1; //or not
    }
  }
}
void  chooseRandomOnLeftSide(byte  pattern [8][8]) {
  for (int i = 0; i < 8; i++) { //tp go through each LED of a col
    if (random() % 6 == 0) { //each LED has a one-in-six chance of being lit, which seems to look the nicest
      pattern[i][0] = 0; //again this does the setting to turn an LED on
    } else {
      pattern[i][0] = 1; //otherwise it stays off
    }
  }
}


//this code is adapted from the initial voltage divider code provided by sparkfun
//https://learn.sparkfun.com/tutorials/flex-sensor-hookup-guide 
double flexToSomething(int pin) {
  double flex = (double)analogRead(pin);
  double vout = (flex + R_DIV) / (VCC * R_DIV);
  double linerizedValue = 1 / vout;
  double correctedValue = map(flex, 39, 297, 50, 500); //I tried a few different methods to lineraize the data and eventually settled on map


  return correctedValue;
  // return linerizedValue;
  //return vout;
}




void loop() {

  iterator++; //counts the loops for direction alternating
  int TIME_DELAY;// = flexToSomething(A4); //time delay for first flex sensor
  int TIME_DELAY2;// = flexToSomething(A5); //tme delay for second flex sensor

  static byte ledOn[8][8]; //establishes 2D array for turning the LEDs on/off
  // randomSeed(10); //for consistency
  static  long  nextUpdateTime = millis (); //manages time/updates for first flex sensor
  static  long  nextUpdateTime2 = millis (); //manages time/updates for second flex seonsor


  //Serial.println(analogRead(FLEX_PIN));

  int flexV = analogRead(FLEX_PIN);
  int flexV2 = analogRead(FLEX_PIN2);
  TIME_DELAY = flexToSomething(FLEX_PIN);
  // TIME_DELAY = flexV;
  TIME_DELAY2 = flexToSomething(FLEX_PIN2);
  //TIME_DELAY2 = flexV2;
  

  display(ledOn); //look the display is on! 



  if (iterator % 2 == 0) { //happens every other loops
    if (millis() > nextUpdateTime + TIME_DELAY) { //decides to update based on flex sensor bend-age, more bent = faster update
      nextUpdateTime = millis();

      movePatternDown(ledOn); //moves the pattern down
//      Serial.println("TIME1: " + String(TIME_DELAY)); //example leftover debugging line


    }
  } else {  //happens the other every other loops

    if (millis() > nextUpdateTime2 + TIME_DELAY2) { //decides to update based on second flex sensor bend-age, more bent = faster update
      nextUpdateTime2 = millis();
      movePatternOver(ledOn); //moves pattern to the right 
     // Serial.println("TIME2: " + String(TIME_DELAY2)); //example leftover debugging line


    }
  }


  //TODO
  /*
     repeat this with a different time delay for second flex sensor
     the second one will controll movePatternOver
  *//*
  if (millis() > nextUpdateTime + TIME_DELAY2) {
  //  nextUpdateTime2=millis();
  //  movePattern(ledOn);
  movePatternOver(ledOn);
  //delay(1000);
  }
  display(ledOn);
  //  flexCheck();
*/

}