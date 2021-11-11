#include <ESP8266WiFi.h> // This includes the libraries to connect to wifi and start a server
#include <PubSubClient.h>

const char* wifiSSID = "eesc"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "password"; // And the preshared key (wifi password)

const char* mqttServer = "192.168.137.115";
const char* ultrasonicSensorTopic = "homeassistant/sensor/distance";
String host = "lights";

int red = D1;
int yellow = D2;
int green = D3;

const char* selectedState = "off";

WiFiClient wifiClient;

PubSubClient mqttClient(wifiClient);

void setup() { // Perform this part once when powered on
 Serial.begin(115200); // Open a serial connection (for debugging)
 delay(10); // Wait 10 milliseconds (1/100th of a second)
 pinMode(red, OUTPUT); // Set the LED pin to Output
 pinMode(yellow, OUTPUT);
 pinMode(green, OUTPUT);
 
 digitalWrite(red, LOW); // Set the pin to LOW (Off)
 digitalWrite(yellow, LOW); // Set the pin to LOW (Off)
 digitalWrite(green, LOW); // Set the pin to LOW (Off)
 
 // ** Connect to WiFi network - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
 Serial.print("Connecting to "); // Display debugging connection info
 Serial.println(wifiSSID); // Print the configured SSID to the Serial Monitor

 WiFi.begin(wifiSSID, wifiPSK); // Use the provided SSID and PSK to connect
 
 while (WiFi.status() != WL_CONNECTED) { // If not connected to wifi
   delay(500); // Pause
   Serial.print("."); // Print a dot each loop while trying to connect
 }
 
 Serial.println("");
 Serial.println("WiFi connected");

 //connecting to a mqtt broker
 Serial.println("Connecting to MQTT");
 mqttClient.setServer(mqttServer, 1883);
 mqttClient.setCallback(callback);
 while (!mqttClient.connected()) {     // Function to call when new messages are received
  mqttClient.connect(host.c_str());     // Connect to the broker with unique hostname
  Serial.println(mqttClient.state());   // Show debug info about MQTT state
 }
 mqttClient.subscribe(ultrasonicSensorTopic);
}

void loop() { 

  mqttClient.loop();
  
  delay(100);
}

void callback(char* topicChar, byte* payload, unsigned int length){
  Serial.println("Received a message" + String(topicChar));

  String message = "";                  // Convert the byte* payload to String
  // There won't be a null character to terminate, so we manually copy each character
  for (int i = 0; i < length; i++) {    // For each character of the payload
    message += (char)payload[i];        // append to the message string
  }

  Serial.println("Payload " + message); 

  lightOn(message);  
}

void lightOn(String light){
  if(light == "red"){
      Serial.println("RED ON");
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(yellow, LOW);
 }
  else if(light == "green"){
    Serial.println("GREEN ON");
    digitalWrite(red, LOW);
    digitalWrite(green, HIGH);
    digitalWrite(yellow, LOW);
  } 
  else if(light == "yellow"){
    Serial.println("YELLOW ON");
    digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    digitalWrite(yellow, HIGH);
  }
  else if(light == "all"){
    Serial.println("ALL ON");
    digitalWrite(red, HIGH);
    digitalWrite(green, HIGH);
    digitalWrite(yellow, HIGH);
  }
  else {
    Serial.println("OFF");
    digitalWrite(red, LOW);
    digitalWrite(green, LOW);
    digitalWrite(yellow, LOW);
  }
}
