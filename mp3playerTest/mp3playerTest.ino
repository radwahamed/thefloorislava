/*
 * Final Project ME216M
 * 
 * MP3 Player Testing Module
 * 
 * This module plays one of 6 available messages:
 *      1. "Setting Up"
 *      2. "Press Button to Start"
 *      3. "Waiting on a friend to start" / "Your friend has joined, press button to start"
 *      4. "Starting"
 *      5. "12"
 *      6. "Player X Wins! Press Button to Restart" 
 *      
*/
 
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


enum audio_state {SETUP = 0, 
                  PRESS_START = 1, 
                  WAITING_ON_FRIEND = 2, 
                  WAITING_ON_YOU = 3,
                  ANNOUNCE_PLAYER_NUMBER_START = 4, 
                  SCORE = 6, 
                  WIN = 20, 
                  LOSE = 21, 
                  RESTART = 22};
audio_state current_audio;

void setup() {
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

}

void loop() {
  // put your main code here, to run repeatedly:
  current_audio = SETUP;
  int param = 0;
  playAudio(param);
  current_audio = PRESS_START;
  playAudio(param);
  current_audio = WAITING_ON_FRIEND;
  playAudio(param);
  current_audio = WAITING_ON_YOU;
  playAudio(param);
  current_audio = ANNOUNCE_PLAYER_NUMBER_START;
  playAudio(param);
  current_audio = ANNOUNCE_PLAYER_NUMBER_START;
  param = 1;
  playAudio(param);
  current_audio = SCORE;
  param = 1;
  playAudio(param);
  current_audio = SCORE;
  param = 2;
  playAudio(param);
  current_audio = SCORE;
  param = 12;
  playAudio(param);
  current_audio = WIN;
  param = 0;
  playAudio(param);
  current_audio = LOSE;
  playAudio(param);
  current_audio = RESTART;
  playAudio(param);
}

void playAudio(int param)
{
  Mp3Player.controller->playSongMP3(current_audio+param);
  delay(30000); 
  Mp3Player.controller->stop();
}
