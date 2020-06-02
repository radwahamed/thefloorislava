/*
 * Final Project ME216M
 * 
 * MP3 Player Testing Module
 * 
 * This module is set to act as the framework for the state machine and the event handling part
 * It is the skeleton for the big project.
 * 
 *      
*/
#include <Wire.h>
#include <EventManager.h>


/*
 *  ----------------------
 * Event & State Variables
 * -----------------------
 */     
#define YOU_JOINED_FIRST EventManager::kEventUser0
#define FRIEND_JOINED_FIRST EventManager::kEventUser1
#define TWO_PLAYER_JOINED EventManager::kEventUser2
#define STEP_SUCCESS EventManager::kEventUser3 //Timer didn’t elapse & right pressure sensor is activated
#define EXCEEDED_SCORE_THRESHOLD EventManager::kEventUser4
#define TURN_SUCCESS EventManager::kEventUser5 //successful step and no level increment or level increment
#define FRIEND_TURN_SUCCESS EventManager::kEventUser6
#define STEP_FAILURE EventManager::kEventUser7
#define FRIEND_TURN_FAILURE EventManager::kEventUser8

// Create the Event Manager
EventManager eventManager;

enum state {INIT,
            WAITING_FOR_YOU, 
            WAITING_FOR_FRIEND, 
            START_GAME, 
            GENERATE_NEXT_STEP, 
            ADVANCE, 
            END_GAME, 
            INCREMEMENT_LEVEL,
            WAITING_FOR_FRIEND_TO_PLAY};
            
state currentState = INIT;

void setup() {
  
  // Initialize serial communication
  Serial.begin(9600); 
  /*
   *  -----------------
   * Event & State INIT
   * ------------------
   */ 
  eventManager.addListener(YOU_JOINED_FIRST, lava_SM);
  eventManager.addListener(FRIEND_JOINED_FIRST, lava_SM);
  eventManager.addListener(TWO_PLAYER_JOINED, lava_SM);
  eventManager.addListener(STEP_SUCCESS, lava_SM);
  eventManager.addListener(EXCEEDED_SCORE_THRESHOLD, lava_SM);
  eventManager.addListener(TURN_SUCCESS, lava_SM);
  eventManager.addListener(FRIEND_TURN_SUCCESS, lava_SM);
  eventManager.addListener(STEP_FAILURE, lava_SM);
  eventManager.addListener(FRIEND_TURN_FAILURE, lava_SM);
  
  lava_SM(INIT,0);
}

void loop() {
  eventManager.processEvent();

  OnJoiningFirst();   
  OnFriendJoiningFirst();
  OnTwoPlayerJoining(); 
  OnStepSucceeding();
  OnExceedingScoreThreshold();
  OnTurnSuccess(); 
  OnFriendTurnSuccess(); 
  OnStepFailure();
  OnFriendTurnFailure();
}
/*
 *  --------------
 * Event Checkers
 * ---------------
 */ 
void OnJoiningFirst(){
     //if start button is pressed
     //if http request friend status return that they have NOT joined
     eventManager.queueEvent(YOU_JOINED_FIRST, 0);
}
void OnFriendJoiningFirst(){
     //if start button is not pressed
     //if http request friend status return that they have joined
     eventManager.queueEvent(FRIEND_JOINED_FIRST, 0);
}
void OnTwoPlayerJoining(){
   // if button pressed and http request friend status return that they have joined
   eventManager.queueEvent(TWO_PLAYER_JOINED, 0);
}
void OnStepSucceeding(){
   // if Timer didn’t elapse & right pressure sensor is activated
   stepSuccess = true;
   eventManager.queueEvent(STEP_SUCCESS, pressedTile);
}
void OnExceedingScoreThreshold(){
  // if score == 5
  eventManager.queueEvent(EXCEEDED_SCORE_THRESHOLD, nextLevel);
  nextLevel++
  // if score == 10
  eventManager.queueEvent(EXCEEDED_SCORE_THRESHOLD, nextLevel);
  
}
void OnTurnSuccess(){
  //if stepSuccess
  eventManager.queueEvent(TURN_SUCCESS, 0);
  
}
void OnFriendTurnSuccess(){
  // if http request frient turn status == True
  eventManager.queueEvent(FRIEND_TURN_SUCCESS, 0);
}
void OnStepFailure(){
  // if Timer elapsed & wrong pressure sensor is activated
  eventManager.queueEvent(STEP_FAILURE, 0);
}
void OnFriendTurnFailure(){
  // if http request frient turn status == False
  eventManager.queueEvent(FRIEND_TURN_FAILURE, 0);
}

void lava_SM( int event, int param )
{ 
  state nextState = currentState;

  switch (currentState) {
    case INIT:
      Serial.println("STATE: Initialization");
      
      // Set Scores to Zero
      // Set Both Players to Not_Joined
      // Set Your Player Number to -1 [are you player 1 or player 2, based on order of joining]
      // set current level to 1
      // Set LCD to welcome player and press start
      // Play Welcome Music
      if ( event == FRIEND_JOINED_FIRST) {
        // Display on LCD Friend Joined Waiting For You
        nextState = WAITING_FOR_YOU;
      }
      if ( event == YOU_JOINED_FIRST) {
        // set player number to 1
        // Display on LCD Waiting on Friend To join
        nextState = WAITING_FOR_FRIEND;
      }
      break;

    case WAITING_FOR_YOU:
      Serial.println("STATE: Waiting For You");
      if ( event == TWO_PLAYER_JOINED) {
        //set player number to 2
        // reset score & start music
        nextState = START_GAME;
      }
      break; 

    case WAITING_FOR_FRIEND:
     Serial.println("STATE: Waiting For Friend");
      if ( event == TWO_PLAYER_JOINED) {
        // reset score & start music
        nextState = START_GAME;
      }
      break;
      
    case START_GAME:
     Serial.println("STATE: Start Game");
     // if player number -> 1
        // Display on LCD it is your turn
        nextState = GENERATE_NEXT_STEP;
     // if player number -> 2
        // Display on LCD it is friend's turn
        nextState = WAITING_FOR_FRIEND_TO_PLAY;
      break;
      
    case GENERATE_NEXT_STEP:
     Serial.println("STATE: Generate Next Step");
      
      // Do a Get Request to know which block is your friend on rn
      // set your friend's tile's red LED to True
      // Generate a random # between 1-5 that is not your current block and not your friend's block
      // Set the Green LED of the next tile  to True
      // start the timer if not already started
      if (event == STEP_SUCCESS) {
        nextState = ADVANCE;
      }
      if (event == STEP_FAILURE) {
        nextState = END_GAME;
      }
      break;
            
    case ADVANCE:
     Serial.println("STATE: Advance");
     // score++
     //display score on LCD
     if (event == EXCEEDED_SCORE_THRESHOLD){
      nextState = INCREMEMENT_LEVEL;
     }
     nextState = WAITING_FOR_FRIEND_TO_PLAY;
      break;
            
    case INCREMEMENT_LEVEL:
     Serial.println("STATE: Increment Level");
     if (param == 2) {
       //set Timer Threshold to 3sec
     } else if(param ==3){
       // set Timer Threshold to 2sec
     }
     nextState = WAITING_FOR_FRIEND_TO_PLAY;
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
      
      // diplay final scores
      // play win/lose audio
      //play score audio
      nextState = INIT;;
      break;

    default:
      Serial.println("STATE: Unknown State");
      break;
  }

  // Update the current state
  currentState = nextState;
}
