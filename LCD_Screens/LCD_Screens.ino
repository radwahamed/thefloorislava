#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

// Leave this line unchanged
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


int score_int = 8;

#define button 17

void setup() {
  Serial.begin(9600);
  LCD_initializations();
  pinMode(button, INPUT);
}


void loop() {
  LCD_settingUp();
  delay(3000);
  LCD_startButton();
  delay(3000);
  LCD_waiting();
  delay(3000);
  LCD_starting();
  delay(3000);
  LCD_jump();
  delay(3000);
  LCD_updateScore();
  delay(1000);
  score_int++;  
  LCD_updateScore();
  delay(1000);
  score_int++;
  LCD_updateScore();
  delay(3000);
  LCD_announceWinner("you");
  delay(3000);
  LCD_announceWinner("friend");
  delay(3000);  
  buttonDown();
}

void LCD_settingUp() { // GOOD
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(2, 32, "Setting Up");
  u8g2.sendBuffer(); 
}


void LCD_jump() { // USED
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

void LCD_meWaiting() { // USED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(5,15, "Waiting on");
  u8g2.drawStr(25,45, "friend...");
  u8g2.sendBuffer();
}

void LCD_friendWaiting() { // USED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(35,15, "Friend");
  u8g2.drawStr(20,45, "waiting...");
  u8g2.sendBuffer();
}

void LCD_announceWinner(String winner) { 
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub14_tf);
  if (winner == "you") {
    u8g2.drawStr(16, 20, "You Win!");
  } else if (winner == "friend") {
    u8g2.drawStr(3, 20, "Friend Wins!");
  }

  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 45, "Press button to play");
  u8g2.drawStr(40, 55, "again");
  u8g2.sendBuffer();
}

void LCD_starting() { // Used
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(10, 32, "Starting...");
  u8g2.sendBuffer();  
}

void LCD_startButton() { // USED
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub14_tf);
  u8g2.drawStr(35, 15, "Press");
  u8g2.drawStr(15, 35, "button to");
  u8g2.drawStr(37, 55, "start");
  u8g2.sendBuffer(); 
}

void buttonDown(){
  static int lastButtonState = LOW;
  int thisButtonState = digitalRead(button);
  // Check if the button has been pressed
  if (thisButtonState != lastButtonState) {
    if (thisButtonState == HIGH) {
      //eventManager.queueEvent(EVENT_BUTTONPRESS, 0);
      Serial.println("Button Pressed");
    }
  }
  lastButtonState = thisButtonState;
}


void LCD_initializations(void){
  u8g2.begin();
  u8g2.setFontRefHeightExtendedText();
  u8g2.setFontDirection(0);
  u8g2.setDrawColor(1);
  u8g2.setFontPosCenter();
}  
