#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

/*
/// HTTP client errors
#define HTTPC_ERROR_CONNECTION_REFUSED (-1)
#define HTTPC_ERROR_SEND_HEADER_FAILED (-2)
#define HTTPC_ERROR_SEND_PAYLOAD_FAILED (-3)
#define HTTPC_ERROR_NOT_CONNECTED (-4)
#define HTTPC_ERROR_CONNECTION_LOST (-5)
#define HTTPC_ERROR_NO_STREAM (-6)
#define HTTPC_ERROR_NO_HTTP_SERVER (-7)
#define HTTPC_ERROR_TOO_LESS_RAM (-8)
#define HTTPC_ERROR_ENCODING (-9)
#define HTTPC_ERROR_STREAM_WRITE (-10)
#define HTTPC_ERROR_READ_TIMEOUT (-11)
*/

const char* wifiSSID = "eesc"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "password"; // And the preshared key (wifi password)

int echo = D5;
int trig = D6;

int sound = D7;
String lightState = "off"; //0 off, 1 red, 2 yellow, 3 green, 4 all

int distance_close = 10;
int distance_mid = 15;
int distance_stop = 5;

long duration = 0;
int distance = 0;
int previous_distance = 0;
int same_distance_timer = 5000; //5s
int same_distance_counter = 0;

String serverAddress = "http://192.168.207.1:80";

//REFERENCES

//ESP8266 HTTP CLIENT
//https://randomnerdtutorials.com/esp8266-nodemcu-http-get-post-arduino/

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
}
void loop() { // ** Create a web server - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
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
  
  if(distance == previous_distance){
    same_distance_counter += 1000;
  }
  else {
    previous_distance = distance;
    same_distance_counter = 0;
    
    lightOn();
    makeRequest();
  }

  if(same_distance_timer <= same_distance_counter){
    Serial.println("TURN OFF");
    if(lightState != "off"){
      lightState = "off";
      makeRequest();
    }
    delay(2000);
    return;
  }
    
  delay(1000);
}

void makeRequest(){
  if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
  
      String serverPath = serverAddress + "/" + lightState;
  
      Serial.println("Calling Server Path with state" + lightState);
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());
  
      // Send HTTP GET request
      int httpResponseCode = http.GET();
  
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      http.end();
    }
}

void lightOn(){
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
