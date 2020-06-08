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
/*
 *  ----------------------
 * Event & State Libraries
 * -----------------------
 */ 
#include <Arduino.h>
#include <Wire.h>
#include <EventManager.h>

/*
 *  -------------
 * MQTT Libraries
 * --------------
 */ 
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include "config.h"

/*    
  * --------------------
  * MP3 Needed Libraries
  * --------------------
*/     
#include "SeeedGroveMP3.h"
#include "KT403A_Player.h"
#include "WT2003S_Player.h"
#include "wiring_private.h"

/*
 * ---------------
 *  LCD Libraries
 * ---------------
 */ 
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

/*
 *  ----------------------
 * Event & State Variables
 * -----------------------
 */     
#define YOU_JOINED EventManager::kEventUser0
#define FRIEND_JOINED EventManager::kEventUser1
#define STEP_SUCCESS EventManager::kEventUser2 //Timer didn’t elapse & right pressure sensor is activated
#define STEP_FAILURE EventManager::kEventUser3
#define FRIEND_TURN_SUCCESS EventManager::kEventUser4
#define FRIEND_TURN_FAILURE EventManager::kEventUser5


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
            
state currentState = INIT;

/*
 * --------------
 * MQTT Variables
 * --------------
 */
char message[1024];
enum players {Idle, Joined, Failed};
players pStatus [2];
int pStep[2];

// WiFi Parameters (defined in config.h)
const char* wifi_ssid = networkSSID;
const char* wifi_password = networkPASSWORD;

// MQTT Parameters (defined in config.h)
const char* mqttServer = ioHOST;            // Adafruit host
const char* mqttUsername = ioUSERNAME;      // Adafruit username
const char* mqttKey = ioKEY;                // Adafruit key
int mqttPort = defaultPORT;                 // Default MQTT Port
const char* subTopic = ioSubFeed;           
const char* pubTopic = ioPubFeed;           
WiFiClient wifiClient;
PubSubClient client(wifiClient);


/*
 *  ---------------
 * Tile Variables
 * ----------------
 */

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

// Initialize based on your pressure sensor
int threshold1 = 400;
int threshold2 = 400;
int threshold3 = 400;
int threshold4 = 400;
int threshold5 = 400;
int sensor1, sensor2, sensor3, sensor4, sensor5;

/*
 *  ---------------
 * Game Variables
 * ----------------
 */
boolean iJoined;
boolean friendJoined;

int friendPlayerNumber = -1;
int myPlayerNumber = -1;
int myNextStep, myCurrentStep, friendStep;
int jumpTimeThreshold /* in milliseconds */, score_int;
unsigned long currentStepTime, currentStepStartTime;
String winner;

/*    
  * --------------------
  * MP3 Player Variables
  * --------------------
*/  

#define COMSerial mp3Serial // the serial port used for UART communication with the mp3 player
Uart mp3Serial (&sercom0, 6, 7, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM0_Handler() 
{
    mp3Serial.IrqHandler();
}
MP3Player<KT403A<Uart>> Mp3Player; // mp3 player object, v2.0

enum audio_state {SETUP, 
                  PRESS_START, 
                  WAITING_ON_FRIEND, 
                  WAITING_ON_YOU,
                  ANNOUNCE_YOU_START, 
                  ANNOUNCE_FRIEND_START, 
                  WIN, 
                  LOSE, 
                  RESTART};
audio_state current_audio;

/*
 *  ---------------
 * LCD Variables
 * ----------------
 */

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

#define ShowSerial SerialUSB // the serial port used for displaying info and reading user input


void setup() {
  Serial.begin(9600);
  /*
   *  -----------------
   * MQTT INIT
   * ------------------
   */ 
   pStatus[0]=Idle;
   pStatus[1]=Idle;
   pStep[0] = -1;
   pStep[1] = -1;
   // Connect to WiFi:
   Serial.print("Connecting to ");
   WiFi.begin(wifi_ssid, wifi_password);
   WiFi.begin(wifi_ssid);    
    
   while (WiFi.status() != WL_CONNECTED) 
   {
     // wait while we connect...
     delay(500);
     Serial.print(".");
   }
   Serial.println("WiFi connected!");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   Serial.println("------------------------");
    
   // Set up MQTT
   client.setServer(mqttServer, mqttPort);
   client.setCallback(getData); // set function to be called when new messages arrive from a subscription

   
   /*
   *  -----------------
   * Tile INIT
   * ------------------
   */ 
   pin_initializations(); // for button, LED, & Pressure sensors
   pinMode(button, INPUT);

   /*    
   * -------------------------
   * MP3 Player INITIALIZATION
   * -------------------------
   */
   pinPeripheral(6, PIO_SERCOM_ALT); 
   pinPeripheral(7, PIO_SERCOM_ALT);
    
   COMSerial.begin(9600);
   while (!COMSerial);
    
   Mp3Player.controller->init(COMSerial); // initialize the MP3 player
   
   /*
   *  -----------------
   * LCD INIT
   * ------------------
   */ 
   LCD_initializations();  
   
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
  // Maintain MQTT Connection
  connectMQTT();
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
     if (digitalRead(button) == HIGH && !iJoined /*to avoid debouncing and multiple presses*/) { // start button pressed
        eventManager.queueEvent(YOU_JOINED, 0);
        iJoined = true;
     }
}

void OnFriendJoining(){
     //if I haven't joined and my friend joined as player one
     //if I have joined and my friend joined as player 2
     if ((myPlayerNumber==-1 && pStatus[0] ==Joined)||(friendPlayerNumber==2 && pStatus[friendPlayerNumber-1] == Joined)) {
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
  //}
}


void OnExceedingScoreThreshold(){ // Does not need to be included in state, can be done globally
  if (score_int == 5){ // LEVEL UP
    jumpTimeThreshold = 3000; /* in millisceonds*/
  } else if (score_int == 10) { // LEVEL UP
    jumpTimeThreshold = 2000; /* in millisceonds*/
  }
}

void checkFriendTurn(){
  if (pStatus[friendPlayerNumber-1]== Failed) {
    eventManager.queueEvent(FRIEND_TURN_FAILURE, 0);
  } else {
    eventManager.queueEvent(FRIEND_TURN_SUCCESS, 0);
  }
}



void lava_SM( int event, int  )
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
      // Play Welcome Music
      current_audio = SETUP;
      playAudio();
      current_audio = PRESS_START;
      playAudio();
      LCD_startButton(); // Tell user to press Button
      
      if (event == FRIEND_JOINED) {
        current_audio = WAITING_ON_YOU;
        playAudio();
        LCD_friendWaiting();
        nextState = WAITING_FOR_YOU;
      }
      if (event == YOU_JOINED) {
        current_audio = WAITING_ON_FRIEND;
        playAudio();
        LCD_meWaiting();
        nextState = WAITING_FOR_FRIEND;
      }
      break;

    case WAITING_FOR_YOU:
      Serial.println("STATE: Waiting For You");
      friendPlayerNumber = 1;
      myPlayerNumber = 2;
      if (event == YOU_JOINED) {
        pStatus[myPlayerNumber-1]  = Joined;
        postData();
        Serial.println("Both Players Joined");
        LCD_gameStarting();
        nextState = START_GAME;
      }
      break; 

    case WAITING_FOR_FRIEND:
      friendPlayerNumber = 2;
      myPlayerNumber = 1;
      pStatus[myPlayerNumber-1] = Joined;
      postData();
      Serial.println("STATE: Waiting For Friend");
      if ( event == FRIEND_JOINED) {
        Serial.println("Both Players Joined");
        LCD_gameStarting();
        nextState = START_GAME;
      }
      break;
      
    case START_GAME:
      Serial.println("STATE: Start Game");
      //start music
      if (myPlayerNumber == 1) { // my turn
        current_audio = ANNOUNCE_YOU_START;
        playAudio();
        LCD_jump();
        nextState = GENERATE_NEXT_STEP;
      } else { // friend's turn
        current_audio = ANNOUNCE_FRIEND_START;
        playAudio();
        nextState = WAITING_FOR_FRIEND_TO_PLAY;
      }
      break;
      
    case GENERATE_NEXT_STEP:
      Serial.println("STATE: Generate Next Step");     
      friendStep = pStep[friendPlayerNumber-1];
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
        pStep[myPlayerNumber-1]=myCurrentStep;
        postData();
        myCurrentStep = myNextStep;
        nextState = WAITING_FOR_FRIEND_TO_PLAY;
      }
      if (event == STEP_FAILURE) {
        pStatus[myPlayerNumber-1]=Failed;
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
     if(winner == "me") {
      current_audio = WIN;
      playAudio();
     } else if(winner == "friend"){
        current_audio = LOSE;
        playAudio();
     }
     nextState = INIT; 
     break;

    default:
      Serial.println("STATE: Unknown State");
      break;
  }

  // Update the current state
  currentState = nextState;
}

/*
 * ----------------------
 * MQTT Utility Functions
 * ----------------------
 */

/* 
 *  Function called when a new message arrives from a subscription.
 * The bytes are unmarshalled into the integer values they correspond to
 */
void getData(char* topic, byte* payload, unsigned int payload_length) {
  
  // Read & print payload
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < payload_length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  byte * bufptr = payload;
  memcpy(&pStatus[0], bufptr, sizeof(pStatus[0]));
  bufptr += sizeof(pStatus[0]); 
  memcpy(&pStatus[1], bufptr, sizeof(pStatus[1]));
  bufptr += sizeof(pStatus[1]); 
  memcpy(&pStep[0], bufptr, sizeof(pStep[0]));
  bufptr += sizeof(pStep[0]);
  memcpy(&pStep[1], bufptr, sizeof(pStep[1]));  
}


/*
 * The bytes are marshalled from the integer values they correspond to
 */
void postData() {
  char * bufptr = message;
  memcpy(bufptr, &pStatus[0], sizeof(pStatus[0]));
  bufptr += sizeof(pStatus[0]); 
  memcpy(bufptr, &pStatus[1], sizeof(pStatus[1]));
  bufptr += sizeof(pStatus[1]);
  memcpy(bufptr, &pStep[0], sizeof(pStep[0]));
  bufptr += sizeof(pStep[0]);
  memcpy(bufptr, &pStep[1], sizeof(pStep[1]));
  client.publish(pubTopic, message);
}

/* Maintain MQTT Connection */
void connectMQTT() {
  
  // If we're not yet connected, reconnect
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Create a random client ID
    String clientId = "ArduinoClient-";
    clientId += String(random(0xffff), HEX);
   
    // Attempt to connect
    if (client.connect(clientId.c_str(), mqttUsername, mqttKey)) {
      // Connection successful
      Serial.println("successful!");

      // Subscribe to desired topics
      client.subscribe(subTopic);
    } 
    else {
      // Connection failed. Try again.
      Serial.print("failed, state = ");
      Serial.print(client.state());
      Serial.println(". Trying again in 5 seconds.");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  // Process incoming messages and maintain connection to MQTT server
  client.loop();
  
}

/*
 * --------------------
 * MP3 Utility Fuctions
 * --------------------
 */

void playAudio()
{
  Mp3Player.controller->playSongMP3(current_audio);
  delay(30000); 
  Mp3Player.controller->stop();
}

/*
 * ---------------------
 * LCD Utility Functions
 * ---------------------
 */


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

/*
 * ------------------------
 * Tile Utility Functions
 * ------------------------
 */

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
