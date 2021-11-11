#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Arduino_JSON.h>

const char* wifiSSID = "eesc"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "password"; // And the preshared key (wifi password)

int echo = D5;
int trig = D6;

int sound = D7;
const char* lightState = "off"; //0 off, 1 red, 2 yellow, 3 green, 4 all

int distance_close = 10;
int distance_mid = 15;
int distance_stop = 5;

long duration = 0;
int distance = 0;

boolean isDoorOpen = false;

const char* mqttServer = "192.168.137.115";
String doorTopic = "homeassistant/switch/garage_door";
String distanceTopic = "homeassistant/sensor/distance";
String host = "ultrasonic";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

//REFERENCES

void setup() { 
  Serial.begin(115200); // Open a serial connection (for debugging)
  delay(10); 
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(sound, OUTPUT);
  digitalWrite(sound, LOW);
  
  // ** Connect to WiFi network - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
  Serial.print("Connecting to "); // Display debugging connection info
  Serial.println(wifiSSID); // Print the configured SSID to the Serial Monitor
  WiFi.begin(wifiSSID, wifiPSK); // Use the provided SSID and PSK to connect
  
  while (WiFi.status() != WL_CONNECTED) { // If not connected to wifi
   delay(500); // Pause
   Serial.print("."); // Print a dot each loop while trying to connect
  }
  
  Serial.println("");
  Serial.println("WiFi connected"); // Print "connected" message to the Serial Monitor
  
  
  //connecting to a mqtt broker
  Serial.println("Connecting to MQTT");
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setCallback(callback);
  while (!mqttClient.connected()) {     // Function to call when new messages are received
  mqttClient.connect(host.c_str());     // Connect to the broker with unique hostname
  Serial.println(mqttClient.state());   // Show debug info about MQTT state
  }
  mqttClient.subscribe(doorTopic.c_str());

  JSONVar json;
  
  json["name"] = "sonic";
  json["unique_id"] = "sonic";
  json["state_topic"] = distanceTopic;
  
  mqttClient.publish((distanceTopic + "/config").c_str(), JSON.stringify(json).c_str());
  Serial.println("finished setup");
}
void loop() {

  mqttClient.loop();

  if(!isDoorOpen){
    Serial.println("Door is closed, do not read data");
    lightState = "off";
  }
  else {
    digitalWrite(sound, LOW);
    digitalWrite(trig, LOW);
    delay(20);
    digitalWrite(trig, HIGH);
    delayMicroseconds(20);
    digitalWrite(trig, LOW);
     // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echo, HIGH);
    // Calculating the distance
    distance = duration * 0.034 / 2; // Speed of sound wave (0.034) divided by 2 (go and back)
    // Displays the distance on the Serial Monitor
    Serial.print("Duration ");
    Serial.println(duration);
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm"); 

    checkDistance();
  }

  mqttClient.publish(distanceTopic.c_str(), lightState);
    
  delay(1000);
}

void checkDistance(){
  if(distance <= distance_stop){
    Serial.println("STOP ON");
    tone(sound, 2000, 500);
    lightState = "all";
  }
  else if(distance <= distance_close){
    Serial.println("RED ON");
    lightState = "red";
  }
  else if(distance <= distance_mid && distance > distance_close){
    Serial.println("YELLOW ON");
    lightState = "yellow";
  }
  else {
    Serial.println("GREEN ON");
    lightState = "green";
  }
}


void callback(char* topicChar, byte* payload, unsigned int length){
  Serial.println("Received a message" + String(topicChar));

  String message = "";                  // Convert the byte* payload to String
  // There won't be a null character to terminate, so we manually copy each character
  for (int i = 0; i < length; i++) {    // For each character of the payload
    message += (char)payload[i];        // append to the message string
  }

  Serial.println("Payload " + message);

  if(message == "open"){
    isDoorOpen = true;
  }
  else {
    isDoorOpen = false;
  }
}
