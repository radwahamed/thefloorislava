/*
 * Final Project ME216M
 * 
 * Skeleton FSM Module
 * 
 * This module is set to act as the framework for the state machine and the event handling part
 * It is the skeleton for the big project.
 * 
 *      
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <EventManager.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define ShowSerial SerialUSB // the serial port used for displaying info and reading user input

/*
 *  ----------------------
 * Event & State Variables
 * -----------------------
 */     
#define YOU_JOINED EventManager::kEventUser0
#define FRIEND_JOINED EventManager::kEventUser1
#define STEP_SUCCESS EventManager::kEventUser3 //Timer didn’t elapse & right pressure sensor is activated
#define STEP_FAILURE EventManager::kEventUser4
#define FRIEND_TURN_SUCCESS EventManager::kEventUser5
#define STEP_FAILURE EventManager::kEventUser6
#define FRIEND_TURN_FAILURE EventManager::kEventUser7

#define button 17
#define P1 A0 // Pressure Sensor on Stone 1
#define T1 2 // Target LED on Stone 1
#define F1 3 // Friend LED on Stone 1
#define P2 A1 
#define T2 4
#define F2 5
#define P3 A2 
#define T3 8
#define F3 9
#define P4 A6
#define T4 10
#define F4 11
#define P5 A7
#define T5 12
#define F5 13


// Create the Event Manager
EventManager eventManager;

enum state {INIT,
            WAITING_FOR_YOU, 
            WAITING_FOR_FRIEND, 
            START_GAME, 
            GENERATE_NEXT_STEP,
            CHECK_STEP,  
            END_GAME, 
            WAITING_FOR_FRIEND_TO_PLAY};

boolean iJoined;
boolean friendJoined;
            
state currentState = INIT;

// Initialize based on your pressure sensor
int threshold1 = 400;
int threshold2 = 400;
int threshold3 = 400;
int threshold4 = 400;
int threshold5 = 400;
int sensor1, sensor2, sensor3, sensor4, sensor5;

int friendPlayerNumber, myPlayerNumber;
int myNextStep, myCurrentStep, friendStep;
int jumpTimeThreshold /* in milliseconds */, score_int;
unsigned long currentStepTime, currentStepStartTime;
String winner;

void setup() {
  Serial.begin(9600);
  LCD_initializations();
  pin_initializations(); // for button, LED, & Pressure sensors
  pinMode(button, INPUT);
  
  
  /*
   *  -----------------
   * Event & State INIT
   * ------------------
   */ 
  eventManager.addListener(YOU_JOINED, lava_SM);
  eventManager.addListener(FRIEND_JOINED, lava_SM);
  eventManager.addListener(STEP_SUCCESS, lava_SM);
  eventManager.addListener(STEP_FAILURE, lava_SM);
  eventManager.addListener(FRIEND_TURN_SUCCESS, lava_SM);
  eventManager.addListener(FRIEND_TURN_FAILURE, lava_SM);
  
  lava_SM(INIT,0);
}

void loop() {
  eventManager.processEvent();

   
  // These should be in a while loop before the game started to keep it from slowing down
  if (iJoined == false) {
    OnJoining(); 
  }
  if (friendJoined == false) {
    OnFriendJoining();
  }
  if(iJoined && friendJoined) { // Both Players Joined
      checkStep();
      OnExceedingScoreThreshold();
      checkFriendTurn();
      
  }

}
/*
 *  --------------
 * Event Checkers
 * ---------------
 */ 
void OnJoining(){
     if (digitalRead(button) == HIGH) { // start button pressed
        eventManager.queueEvent(YOU_JOINED, 0);
        iJoined = true;
     }
}

void OnFriendJoining(){
     //if http request friend status return that they have joined
     if (//if http request friend status return that they have joined) {
        eventManager.queueEvent(FRIEND_JOINED, 0);
        friendJoined = true;
     }
}


void checkStep() {
  // Case for Step Failure
  currentStepTime = millis() - currentStepStartTime;
  //if (currentStepTime > jumpTimeThreshold) {
    // Read Pressure Sensor Data after jumpTime
    // if (readMyStep(myNextStep)) { // returns true if only next step is being stepped on
    //      eventManager.queueEvent(STEP_SUCCESS, 0)
    // } else {
    //      eventManager.queueEvent(STEP_FAILURE, 0);
    // }
  //} else { // step time is less jumpTimeThreshold
    // If no tiles are pressed, this is a failure bc you are touching lava
    // if (readMyStep(0)) { // function returns true if none are pressed
    //    eventManager.queueEvent(STEP_FAILURE, 0);
    // }
  }
}


void OnExceedingScoreThreshold(){ // Does not need to be included in state, can be done globally
  if (score_int == 5){ // LEVEL UP
    jumpTimeThreshold = 3000; /* in millisceonds*/
  } else if (score_int == 10) { // LEVEL UP
    jumpTimeThreshold = 2000; /* in millisceonds*/
  }
}

void checkFriendTurn(){
  // read http request friend turn status
  // if it equals true
    eventManager.queueEvent(FRIEND_TURN_SUCCESS, 0);
  // if false
    eventManager.queueEvent(FRIEND_TURN_FAILURE, 0);
}



void lava_SM( int event, int param )
{ 
  state nextState = currentState;

  switch (currentState) {
    case INIT:
      Serial.println("STATE: Initialization");
      
      // Set Scores to Zero
      score_int = 0;
      // Set Both Players to Not_Joined
      iJoined = false;
      friendJoined = false;
      jumpTimeThreshold = 5000;
      myPlayerNumber = -1; /*I didn't join*/
      friendPlayerNumber = -1; /*My friend didn't join*/
      
      LCD_startButton(); // Tell user to press Button
      
      // Play Welcome Music
      if (event == FRIEND_JOINED) {
        // update local variable p1Status = Joined
        LCD_friendWaiting();
        nextState = WAITING_FOR_YOU;
      }
      if (event == YOU_JOINED) {
        LCD_meWaiting();
        nextState = WAITING_FOR_FRIEND;
      }
      break;

    case WAITING_FOR_YOU:
      Serial.println("STATE: Waiting For You");
      friendPlayerNumber = 1;
      myPlayerNumber = 2;
      if (event == YOU_JOINED) {
        Serial.println("Both Players Joined");
        LCD_gameStarting();
        //publish p2Status Joined
        nextState = START_GAME;
      }
      break; 

    case WAITING_FOR_FRIEND:
      friendPlayerNumber = 2;
      myPlayerNumber = 1;
      Serial.println("STATE: Waiting For Friend");
      //publish p1Status Joined
      if ( event == FRIEND_JOINED) {
        Serial.println("Both Players Joined");
        LCD_gameStarting();
        // update local variable p2Status = Joined
        nextState = START_GAME;
      }
      break;
      
    case START_GAME:
      Serial.println("STATE: Start Game");
      //start music
      if (myPlayerNumber == 1) { // my turn
        LCD_jump();
        // mp3 "You Start"
        nextState = GENERATE_NEXT_STEP;
      } else { // friend's turn
        // mp3 "Friend Starts"
        nextState = WAITING_FOR_FRIEND_TO_PLAY;
      }
      break;
      
    case GENERATE_NEXT_STEP:
      Serial.println("STATE: Generate Next Step");     
      // Do a Get Request to know which block is your friend on rn
     
      lightFriendStep(friendStep); // Light friend's step

      // Loop generates a random # between 1&5 that is not myCurrentStep or friendStep
      while ((myNextStep==myCurrentStep) || (myNextStep==friendStep)){
        myNextStep = random(1,6);
      }

      lightMyStep(myNextStep); // Light my next Step

      currentStepStartTime = millis();

      nextState = CHECK_STEP;

    case CHECK_STEP:
      Serial.println("STATE: Check Step");
      if ((currentStepTime > jumpTimeThreshold) && (event == STEP_SUCCESS)) {
        score_int++;
        LCD_updateScore();
        // send my HTTP Success and Current Tile Location
        myCurrentStep = myNextStep;
        nextState = WAITING_FOR_FRIEND_TO_PLAY;
      }
      if (event == STEP_FAILURE) {
        // send my HTTP Failure
        nextState = END_GAME;
        winner = "friend";
      } 
      break;
                      
    case WAITING_FOR_FRIEND_TO_PLAY:
      Serial.println("STATE: Waiting For Friend To Play");
      if (event == FRIEND_TURN_SUCCESS) {
        nextState = GENERATE_NEXT_STEP;
      }
      if (event == FRIEND_TURN_FAILURE) {
        nextState = END_GAME;
        winner = "me";
      }
      break;
            
    case END_GAME:
     Serial.println("STATE: End Game");
     LED_off();
     LCD_announceWinner(winner);
     // play win/lose audio
     // play score audio
     nextState = INIT; 
     break;

    default:
      Serial.println("STATE: Unknown State");
      break;
  }

  // Update the current state
  currentState = nextState;
}




// --------------------------------------
// LCD Functions
// --------------------------------------

void LCD_initializations(void){
  u8g2.begin();
  u8g2.setFontRefHeightExtendedText();
  u8g2.setFontDirection(0);
  u8g2.setDrawColor(1);
  u8g2.setFontPosCenter();
  LCD_settingUp();
}  

void LCD_settingUp() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(2, 32, "Setting Up");
  u8g2.sendBuffer(); 
}

void LCD_startButton() { 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.drawStr(35, 15, "Press");
  u8g2.drawStr(15, 35, "button to");
  u8g2.drawStr(37, 55, "start");
  u8g2.sendBuffer(); 
}

void LCD_meWaiting() { 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(5,15, "Waiting on");
  u8g2.drawStr(25,45, "friend...");
  u8g2.sendBuffer();
}

void LCD_friendWaiting() { 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(35,15, "Friend");
  u8g2.drawStr(20,45, "waiting...");
  u8g2.sendBuffer();
}

void LCD_gameStarting() { 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(10, 32, "Starting...");
  u8g2.sendBuffer();  
}

void LCD_jump() { 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub25_tf);
  u8g2.drawStr(12, 32, "Jump!");
  u8g2.sendBuffer();
}

void LCD_updateScore() { 
  int centered_x;
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub42_tn);
  String score = String(score_int);
  Serial.println(score.length());
  if (score.length() == 1) {
    centered_x = 45;
  } else {
    centered_x = 30;
  }
  u8g2.drawStr(centered_x, 32, score.c_str());
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(25, 60, "Friend's Turn");
  u8g2.sendBuffer();
}

void LCD_announceWinner(String winner) { 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub14_tf);
  if (winner == "me") {
    u8g2.drawStr(16, 20, "You Win!");
  } else if (winner == "friend") {
    u8g2.drawStr(3, 20, "Friend Wins!");
  }

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 45, "Press button to play");
  u8g2.drawStr(40, 55, "again");
  u8g2.sendBuffer();
}

// ------------------------------------------------- LCD FUNCTIONS END

void pin_initializations () {
  // Button initialization
  pinMode(button, INPUT);

  // LED initializations
  pinMode(T1, OUTPUT);
  pinMode(F1, OUTPUT);  
  pinMode(T2, OUTPUT);
  pinMode(F2, OUTPUT);  
  pinMode(T3, OUTPUT);
  pinMode(F3, OUTPUT);
  pinMode(T4, OUTPUT);
  pinMode(F4, OUTPUT);
  pinMode(T5, OUTPUT);
  pinMode(F5, OUTPUT);

  // Pressure Sensor Initializations
  pinMode(P1, INPUT);
  pinMode(P2, INPUT);
  pinMode(P3, INPUT);
  pinMode(P4, INPUT);
  pinMode(P5, INPUT);

}

// -----------------------------------
// LED CODE
// -----------------------------------

void lightMyStep(int myNextStep){
  switch(myNextStep) {
    case 1: {
      digitalWrite(T1, HIGH);
      digitalWrite(T2, LOW);
      digitalWrite(T3, LOW);
      digitalWrite(T4, LOW);
      digitalWrite(T5, LOW);
      break;
    }
    case 2: {
      digitalWrite(T1, LOW);
      digitalWrite(T2, HIGH);
      digitalWrite(T3, LOW);
      digitalWrite(T4, LOW);
      digitalWrite(T5, LOW);
      break;
    }
    case 3: {
      digitalWrite(T1, LOW);
      digitalWrite(T2, LOW);
      digitalWrite(T3, HIGH);
      digitalWrite(T4, LOW);
      digitalWrite(T5, LOW);
      break;
    }
    case 4: {
      digitalWrite(T1, LOW);
      digitalWrite(T2, LOW);
      digitalWrite(T3, LOW);
      digitalWrite(T4, HIGH);
      digitalWrite(T5, LOW);
      break;
    }
    case 5: {
      digitalWrite(T1, LOW);
      digitalWrite(T2, LOW);
      digitalWrite(T3, LOW);
      digitalWrite(T4, LOW);
      digitalWrite(T5, HIGH);
      break;
    }
  }
}

void lightFriendStep(int friendStep){
  switch(friendStep){
    case 1: {
      digitalWrite(F1, HIGH);
      digitalWrite(F2, LOW);
      digitalWrite(F3, LOW);
      digitalWrite(F4, LOW);
      digitalWrite(F5, LOW);
      break;
    }
    case 2: {
      digitalWrite(F1, LOW);
      digitalWrite(F2, HIGH);
      digitalWrite(F3, LOW);
      digitalWrite(F4, LOW);
      digitalWrite(F5, LOW);
      break;
    }
    case 3: {
      digitalWrite(F1, LOW);
      digitalWrite(F2, LOW);
      digitalWrite(F3, HIGH);
      digitalWrite(F4, LOW);
      digitalWrite(F5, LOW);
      break;
    }
    case 4: {
      digitalWrite(F1, LOW);
      digitalWrite(F2, LOW);
      digitalWrite(F3, LOW);
      digitalWrite(F4, HIGH);
      digitalWrite(F5, LOW);
      break;
    }
    case 5: {
      digitalWrite(F1, LOW);
      digitalWrite(F2, LOW);
      digitalWrite(F3, LOW);
      digitalWrite(F4, LOW);
      digitalWrite(F5, HIGH);
      break;
    } 
  }
}

void LED_off() {
  digitalWrite(T1, LOW);
  digitalWrite(T2, LOW);
  digitalWrite(T3, LOW);
  digitalWrite(T4, LOW);
  digitalWrite(T5, LOW);
  digitalWrite(F1, LOW);
  digitalWrite(F2, LOW);
  digitalWrite(F3, LOW);
  digitalWrite(F4, LOW);
  digitalWrite(F5, LOW);
}

// ----------------------------------- LED CODE ENDS

// -----------------------------------
// PRESSURE SENSOR CODE
// -----------------------------------

//void readSensors(void){
//  sensor1 = analogRead(P1);
//  delay(10);
//  sensor2 = analogRead(P2);
//  delay(10);
//  sensor3 = analogRead(P3);
//  delay(10);
//  sensor4 = analogRead(P4);
//  delay(10);
//  sensor5 = analogRead(P5);
//  delay(10);
//}
//
//// Function for after Jump Time Threshold
//// Returns true if only stepping on correct step, false otherwise
//boolean readMyStep(int myNextStep) { 
//  readSensors();
//  switch (myNextStep){
//    case 1: {
//      if ((sensor1>threshold1) && (sensor2<threshold2) && 
//      (sensor3<threshold3) && (sensor4<threshold4) && (sensor5<threshold5)) {
//        return true;
//      } 
//      break;
//    }
//    case 2: {
//      if ((sensor1<threshold1) && (sensor2>threshold2) && 
//      (sensor3<threshold3) && (sensor4<threshold4) && (sensor5<threshold5)) {
//        return true;
//      } 
//      break;
//    }
//    case 3: {
//      if ((sensor1<threshold1) && (sensor2<threshold2) && 
//      (sensor3>threshold3) && (sensor4<threshold4) && (sensor5<threshold5)) {
//        return true;
//      } 
//      break;
//    }
//    case 4: {
//      if ((sensor1<threshold1) && (sensor2<threshold2) && 
//      (sensor3<threshold3) && (sensor4>threshold4) && (sensor5<threshold5)) {
//        return true;
//      } 
//      break;
//    }
//    case 5: {
//      if ((sensor1<threshold1) && (sensor2<threshold2) && 
//      (sensor3<threshold3) && (sensor4<threshold4) && (sensor5>threshold5)) {
//        return true;
//      } 
//      break;
//    }
//  }
//  // otherwise
//  return false;
//}

//------------------------------------------------- Pressure sensor code end
