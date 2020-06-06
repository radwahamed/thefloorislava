/* 
To initialize Pressure Threhold Sensors
*/

#define P1 A0 // Pressure Sensor on Stone 1
#define P2 A1
#define P3 A2
#define P4 A6
#define P5 A7

// Initialize based on your pressure sensor
int threshold1 = 400;
int threshold2 = 400;
int threshold3 = 400;
int threshold4 = 0;
int threshold5 = 400;


void setup() {

  Serial.begin(9600);
  
  // Pressure Sensor Initializations
  pinMode(P1, INPUT);
  pinMode(P2, INPUT);
  pinMode(P3, INPUT);
  pinMode(P4, INPUT);
  pinMode(P5, INPUT);

}

void loop() {

  // Change through pins as you assign threshold values
  findPinThreshold(4);
  delay(100);
  // View on serial monitor or plotter to see assign a threshold value for when you step on it

// STONE 1 Try new sensor.
//So, pull down resistor.
  
  // After Threshold Assigned, test it and comment previous function
  // testPin(5);

}

void findPinThreshold(int stoneNumber) {
  switch (stoneNumber) {
    case 1: {
      int value1 = analogRead(P1);
      Serial.println(value1); 
      }
      break;
    case 2: {
      int value2 = analogRead(P2);
      Serial.println(value2); 
      }
      break;
    case 3: {
      int value3 = analogRead(P3);
      Serial.println(value3);
      }
      break;
    case 4: {
      int value4 = analogRead(P4);
      Serial.println(value4); 
      }
      break;
    case 5: {
      int value5 = analogRead(P5);
      Serial.println(value5); 
      }
      break;
  }
}

void testPin(int stoneNumber) {
  switch (stoneNumber){
    case 1:
    {
      int value1 = analogRead(P1);
      if (value1 > threshold1) {
        Serial.println("Stone 1 has been stepped on");
      }
    }
      break;
    case 2:
    {
      int value2 = analogRead(P2);
      if (value2 > threshold2) {
        Serial.println("Stone 12 has been stepped on");
      }
    }
      break;
    case 3:
    {
      int value3 = analogRead(P3);
      if (value3 > threshold3) {
        Serial.println("Stone 3 has been stepped on");
      }
    }
      break;
    case 4:
    {
      int value4 = analogRead(P4);
      if (value4 > threshold4) {
        Serial.println("Stone 4 has been stepped on");
      }
    }
      break;
    case 5:
    {
      int value5 = analogRead(P5);
      if (value5 > threshold5) {
        Serial.println("Stone 5 has been stepped on");
      }
    }
      break;
  }
}
