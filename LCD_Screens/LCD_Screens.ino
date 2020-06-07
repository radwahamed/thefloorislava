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
String winner = "Radwa";


void setup() {
  Serial.begin(9600);
  u8g2.begin();
  u8g2.setFontRefHeightExtendedText();
  u8g2.setFontDirection(0);
  u8g2.setDrawColor(1);
  u8g2.setFontPosCenter();
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
  LCD_updateScore();
  delay(1000);
  score_int++;  
  LCD_updateScore();
  delay(1000);
  score_int++;
  LCD_updateScore();
  delay(1000);
  LCD_announceWinner();
  delay(3000);
  winner = "Ryanne";
  LCD_announceWinner();
  delay(3000);
}

void LCD_settingUp() { // GOOD
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(2, 32, "Setting Up");
  u8g2.sendBuffer(); 
}

void LCD_updateScore() { // GOOD
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
  u8g2.sendBuffer();
}

void LCD_waiting() { // GOOD
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(5,15, "Waiting on");
  u8g2.drawStr(25,45, "friend...");
  u8g2.sendBuffer();
}

void LCD_announceWinner() { // GOOD
  u8g2.clearBuffer();
  int centered_x;
  String announcement = winner + " Wins!";

  if (winner.length() <= 5) {
    u8g2.setFont(u8g2_font_fub14_tf);
    centered_x = 0;
  } else if (winner.length() <= 8) {
    u8g2.setFont(u8g2_font_fub11_tf);
    centered_x = 10;
  }
  
  u8g2.drawStr(centered_x, 20, announcement.c_str());
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 45, "Press button to play");
  u8g2.drawStr(40, 55, "again");
  u8g2.sendBuffer();
}

void LCD_starting() { // GOOD
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_fub17_tf);
  u8g2.drawStr(10, 32, "Starting...");
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
