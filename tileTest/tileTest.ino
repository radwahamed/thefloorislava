/* 
LED & Pressure Sensor Code Test
*/

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

int myNextStep, myCurrentStep, friendStep;



void setup() {

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

void loop() {

    lightMyStep(1);
    lightFriSendStep(1);
    delay(1000);
    lightMyStep(2);
    lightFriendStep(2);
    delay(1000);  
    lightMyStep(3);
    lightFriendStep(3);
    delay(1000);
    lightMyStep(4);
    lightFriendStep(4);
    delay(1000);
    lightMyStep(5);
    lightFriendStep(5);
    delay(1000);  


  //readMyStep(myCurrentStep, myNextStep);

  
  if (readMyStep(1)) {
    Serial.println("1");
  }
  if (readMyStep(2)) {
    Serial.println("2");
  }
  if (readMyStep(3)) {
    Serial.println("3");
  }
  if (readMyStep(4)) {
    Serial.println("4");
  }
  if (readMyStep(5)) {
    Serial.println("5");
  }
  
}


boolean readMyStep(int myNextStep) { // returns True if correct Step, false if not correct

  readSensors();

  switch (myNextStep){
    case 1: {
      if ((sensor1>threshold1) && (sensor2<threshold2) && 
      (sensor3<threshold3) && (sensor4<threshold4) && (sensor5<threshold5)) {
        return true;
      } 
      break;
    }
    case 2: {
      if ((sensor1<threshold1) && (sensor2>threshold2) && 
      (sensor3<threshold3) && (sensor4<threshold4) && (sensor5<threshold5)) {
        return true;
      } 
      break;
    }
    case 3: {
      if ((sensor1<threshold1) && (sensor2<threshold2) && 
      (sensor3>threshold3) && (sensor4<threshold4) && (sensor5<threshold5)) {
        return true;
      } 
      break;
    }
    case 4: {
      if ((sensor1<threshold1) && (sensor2<threshold2) && 
      (sensor3<threshold3) && (sensor4>threshold4) && (sensor5<threshold5)) {
        return true;
      } 
      break;
    }
    case 5: {
      if ((sensor1<threshold1) && (sensor2<threshold2) && 
      (sensor3<threshold3) && (sensor4<threshold4) && (sensor5>threshold5)) {
        return true;
      } 
      break;
    }
  }
  // otherwise
  return false;
  
}

void readSensors(void){
  sensor1 = analogRead(P1);
  //Serial.print("Sensor1: ");
  //Serial.println(sensor1);
  delay(10);
  sensor2 = analogRead(P2);
  //Serial.print("Sensor2: ");
  //Serial.println(sensor2);
  delay(10);
  sensor3 = analogRead(P3);
  //Serial.print("Sensor3: ");
  //Serial.println(sensor3);
  delay(10);
  sensor4 = analogRead(P4);
  //Serial.print("Sensor4: ");
  //Serial.println(sensor4);
  delay(10);
  sensor5 = analogRead(P5);
  //Serial.print("Sensor5: ");
  //Serial.println(sensor5);
  delay(10);
}

boolean readMyStep(int myCurrentStep, int myNextStep) { // returns True if correct Step, false if not correct
  // if any other step besides myCurrentStep or myNextStep is pressed
  if ((readMyStep(myCurrentStep)) || (readMyStep(myNextStep))) {
    return true;
  }
  return false;
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
