#include <ESP8266WiFi.h> // This includes the libraries to connect to wifi and start a server
const char* wifiSSID = "eesc"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "password"; // And the preshared key (wifi password)
int red = D1;
int yellow = D2;
int green = D3;

const char* selectedState = "off";

//**REFERENCES**
//SERVER CODE: The entire server code was taken from the example lab provided by the instructor in class
//STATIC IP: https://randomnerdtutorials.com/esp8266-nodemcu-static-fixed-ip-address-arduino/

// Set your Static IP address
IPAddress local_IP(192, 168, 207, 1);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);

WiFiServer server(80); // This sets which port our server will listen on

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

 // Configures static IP address
if (!WiFi.config(local_IP, gateway, subnet)) {
  Serial.println("STA Failed to configure");
}
 
 WiFi.begin(wifiSSID, wifiPSK); // Use the provided SSID and PSK to connect
 
 while (WiFi.status() != WL_CONNECTED) { // If not connected to wifi
   delay(500); // Pause
   Serial.print("."); // Print a dot each loop while trying to connect
 }
 
 Serial.println("");
 Serial.println("WiFi connected"); // Print "connected" message to the Serial Monitor
 server.begin(); // Start the web server
 Serial.println("Server started");
 Serial.print("Use this URL : "); // Print the connected IP address to the Serial Monitor
 Serial.print("http://");
 Serial.print(WiFi.localIP());
 Serial.println("/");
} // ** End Adapted Code - This is the end of the code that was adapted from www.esplearning.com

void loop() { // ** Create a web server - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
  WiFiClient client = server.available(); // Create a new client object for available connections
  
  if (client) {                           // If a client is connected, wait until it sends some data
    while (!client.available()) {         // If the client hasn't sent info, wait for it
      delay(10);
    }

    String request = client.readStringUntil('\r');  // read the first line of the request
    Serial.println(request);              // DEBUG CODE - Echo the request to the Serial Monitor for debug
    client.flush();                       // Wait until the buffers are clear

    if (request.indexOf("/red") != -1) {   // If the request is for the page "/red"
      lightOn(1);             //   then set the red pin to high (turn on) do the rest for the other lights
      selectedState = "red";
    }
    else if(request.indexOf("/yellow") != -1) {
      lightOn(3);
      selectedState = "yellow";
    }
    else if(request.indexOf("/green") != -1) {
      lightOn(2);
      selectedState = "green";
    }
    else if(request.indexOf("/all") != -1){
      Serial.println("all");
      selectedState = "all";
      lightOn(4);
    }
    else if(request.indexOf("/off") != -1) {  // If the request is for the page "/off"
      lightOn(0);
      selectedState = "off";                       //   and track the state as "off"
    }
  }
  delay(100);
}

//0 for off
//light has values 1 2 3 for red green yellow
void lightOn(int light){
  switch(light){
    case 1: //red
      Serial.println("RED ON");
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(yellow, LOW);
    break;
    case 2: //green
      Serial.println("GREEN ON");
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(yellow, LOW);
    break;
    case 3: //yellow
      Serial.println("YELLOW ON");
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(yellow, HIGH);
    break;
    case 4: //all
      Serial.println("ALL ON");
      digitalWrite(red, HIGH);
      digitalWrite(green, HIGH);
      digitalWrite(yellow, HIGH);
    break;
    default: //off
      Serial.println("OFF");
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(yellow, LOW);
    break;
  }
}
