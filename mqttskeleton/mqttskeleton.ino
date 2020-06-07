/*
 *  MQTT Skeleton to publish updates to players status and scores
 *  MQTT Topics Players Subscribe to is floorLava
 *  It has a JSON object with the following fields:
 *  p1Status, p2Status, p1Step, p2Step, p1Score, p2Score
 *  Possible Values for Status {Idle, Joined, Failed}
 *
*/

#include <ArduinoJson.h>
#include <WiFiNINA.h> 
#include <PubSubClient.h>
#include "config.h"

//----------------------------------------------
//      Variables & Definitions    
//----------------------------------------------
char message[1024];
enum pStatus {Idle, Joined, Failed};
pStatus p1Status=Idle;
pStatus p2Status=Idle;
int p1Score = 0;
int p2Score = 0;
int p1Step = -1;
int p2Step = -1;

// WiFi Parameters (defined in config.h)
const char* wifi_ssid = networkSSID;
const char* wifi_password = networkPASSWORD;

// MQTT Parameters (defined in config.h)
const char* mqttServer = ioHOST;            // Adafruit host
const char* mqttUsername = ioUSERNAME;      // Adafruit username
const char* mqttKey = ioKEY;                // Adafruit key
int mqttPort = defaultPORT;                 // Default MQTT Port
const char* subTopic = ioSubFeed;           //payload[0] (received) will control/set LED
const char* pubTopic = ioPubFeed;           //payload[0] (sent) should be set to current counter value
WiFiClient wifiClient;
PubSubClient client(wifiClient);

// General variables
long lastMsgTime = 0;
int counter = 0;

//---------------------------------
//              Setup  
//---------------------------------
void setup() 
{  
  Serial.begin(9600);
  delay(2000);

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
}

//---------------------------------
//           Main Loop  
//---------------------------------
void loop() 
{ 
  // Maintain MQTT Connection
  connectMQTT();
}

//---------------------------------
//        Other Functions  
//---------------------------------

/* Function called when a new message arrives from a subscription.
 * Inputs:  - Pointer to topic (char array)
 *          - Pointer to payload (Byte array)
 *          - Length of payload byte array
 * Note: These inputs are automatically generated from newly received 
 *       subscription message. This function should manage receiving 
 *       messages from all subscribed topics.
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
  memcpy(&p1Status, bufptr, sizeof(p1Status));
  bufptr += sizeof(p1Status); 
  memcpy(&p2Status, bufptr, sizeof(p2Status));
  bufptr += sizeof(p2Status); 
  memcpy(&p1Score, bufptr, sizeof(p1Score));
  bufptr += sizeof(p1Score); 
  memcpy(&p2Score, bufptr, sizeof(p2Score));
  bufptr += sizeof(p2Score); 
  memcpy(&p1Step, bufptr, sizeof(p1Step));
  bufptr += sizeof(p1Step);
  memcpy(&p2Step, bufptr, sizeof(p2Step));  
}

void postData() {
  char * bufptr = message;
  memcpy(bufptr, &p1Status, sizeof(p1Status));
  bufptr += sizeof(p1Status); 
  memcpy(bufptr, &p2Status, sizeof(p2Status));
  bufptr += sizeof(p2Status); 
  memcpy(bufptr, &p1Score, sizeof(p1Score));
  bufptr += sizeof(p1Score); 
  memcpy(bufptr, &p2Score, sizeof(p2Score));
  bufptr += sizeof(p2Score); 
  memcpy(bufptr, &p1Step, sizeof(p1Step));
  bufptr += sizeof(p1Step);
  memcpy(bufptr, &p2Step, sizeof(p2Step));
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
