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
#include <Wire.h>
#include <EventManager.h>


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

int friendPlayerNumber, myPlayerNumber, myNextStep, myCurrentStep, friendStep;
int jumpTimeThreshold /* in milliseconds */, int_score;
unsigned long currentStepTime, currentStepStartTime;

void setup() {
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
     //if start button is pressed
     if (//if start button is pressed) {
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
  if (currentStepTime > jumpTimeThreshold) {
	  // Read pressure sensorData (myNextStep) --> only one allowed after timer ticked
	  // if not pressed then 
	  // eventManager.queueEvent(STEP_FAILURE, 0);
	  // else [i.e. it was pressed right when timer ticked]
	  // eventManager.queueEvent(STEP_SUCCESS, 0);
  } else {
  	  // Read pressure sensorData (myNextStep) --> any tile can be stepped onto as long as timer hasn't ticked but only
	  // this is a win
	  // eventManager.queueEvent(STEP_SUCCESS, 0);
  }

}


void OnExceedingScoreThreshold(){ // Does not need to be included in state, can be done globally
  if (int_score == 5){ // LEVEL UP
    jumpTimeThreshold = 3000; /* in millisceonds*/
  } else if (int_score == 10) { // LEVEL UP
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
      int_score = 0;
      // Set Both Players to Not_Joined
      iJoined = false;
      friendJoined = false;
      jumpTimeThreshold = 5000;
      myPlayerNumber = -1; /*I didn't join*/
      friendPlayerNumber = -1; /*My friend didn't join*/
      // Set LCD to welcome player and press start
      // Play Welcome Music
      if (event == FRIEND_JOINED) {
        // Display on LCD Friend Joined Waiting For You
        nextState = WAITING_FOR_YOU;
      }
      if (event == YOU_JOINED) {
        // Display on LCD Waiting on Friend To join
        nextState = WAITING_FOR_FRIEND;
      }
      break;

    case WAITING_FOR_YOU:
      Serial.println("STATE: Waiting For You");
      friendPlayerNumber = 1;
      myPlayerNumber = 2;
      if (event == YOU_JOINED) {
        Serial.println("Both Players Joined");
        // reset score & start music
        nextState = START_GAME;
      }
      break; 

    case WAITING_FOR_FRIEND:
      friendPlayerNumber = 2;
      myPlayerNumber = 1;
      Serial.println("STATE: Waiting For Friend");
      if ( event == FRIEND_JOINED) {
        Serial.println("Both Players Joined");
        // reset score & start music
        nextState = START_GAME;
      }
      break;
      
    case START_GAME:
      Serial.println("STATE: Start Game");
      if (myPlayerNumber == 1) { // my turn
        // Display on LCD that it is your turn "Jump"
        // Generate and light up next step
        nextState = GENERATE_NEXT_STEP;
      } else { // friend's turn
        // Display on LCD waiting "Friend's turn"
        nextState = WAITING_FOR_FRIEND_TO_PLAY;
      }
      break;
      
    case GENERATE_NEXT_STEP:
      Serial.println("STATE: Generate Next Step");     
      // Do a Get Request to know which block is your friend on rn
      // set your friend's tile's green LED to True

      // Loop generates a random # between 1&5 that is not myCurrentStep or friendStep
      while ((myNextStep==myCurrentStep) || (myNextStep==friendStep)){
        myNextStep = random(1,6);
      }

      // Set the red LED of the next tile  to True and others to False - Changed to Red bc Target Image
      // lightMyStep(myNextStep);

      currentStepStartTime = millis();

      nextState = CHECK_STEP;

    case CHECK_STEP:
      Serial.println("STATE: Check Step");
      if (event == STEP_SUCCESS) {
        int_score++;
        // display score on LCD
        // send my HTTP Success and Current Tile Location
        myCurrentStep = myNextStep;
        nextState = WAITING_FOR_FRIEND_TO_PLAY;
      }
      if (event == STEP_FAILURE) {
        // send my HTTP Failure
        nextState = END_GAME;
      } 
      break;
                      
    case WAITING_FOR_FRIEND_TO_PLAY:
      Serial.println("STATE: Waiting For Friend To Play");
      
      if (event == FRIEND_TURN_SUCCESS)
        nextState = GENERATE_NEXT_STEP;
      if (event == FRIEND_TURN_FAILURE)
        nextState = END_GAME;
      break;
            
    case END_GAME:
     Serial.println("STATE: End Game");
      
      // diplay final scores & press button to begin
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
