#include <ESP8266WiFi.h> // This includes the libraries to connect to wifi and start a server
const char* wifiSSID = "eesc"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "password"; // And the preshared key (wifi password)
int red = D1;
int yellow = D2;
int green = D3;

int autoFreq = 1000;                   // How often should we change states
unsigned long autoTimer = 0;           // this is a timer variable
int autoState = 0;                     // variable for checking light
const char* currentColor = "black";

const char* selectedState = "off";

//**REFERENCES**
//SERVER CODE: The entire server code was taken from the example lab provided by the instructor in class
//HTML CODE: The html code was based off the example lab provided by the instructor, but some changes were made to match new logic
//NON-BLOCKING CODE: The non-blocking code was based on the instruction provided in the class slides and arduino demos in learning suite

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
    else if(request.indexOf("/auto") != -1){
      Serial.println("running auto");
      currentColor = "blue";
      selectedState = "auto";
      autoState = 0;
    }
    else if(request.indexOf("/off") != -1) {  // If the request is for the page "/off"
      lightOn(0);
      currentColor = "black";
      autoState = 0;
      selectedState = "off";                       //   and track the state as "off"
    }
    // ** End Adapted Code - FROM EXAMPLE LAB WHICH WAS ADAPTED FROM www.esp8266learning.com

    // Return the response
    printHTML(client);
  }
  else {
    if(selectedState == "auto"){
      if(autoState < 3){
        if(autoTimer < millis()){
          autoState++;
          //Serial.printf("light state %d \n", autoState);
          //Serial.printf("auto timer %d and millis %d \n", autoTimer, millis());
          switch(autoState){
            case 1:
              autoTimer = millis() + autoFreq;
            break;
            case 2: 
              autoTimer = millis() + autoFreq;
            break;
            case 3: 
              autoTimer = millis() + autoFreq;
            break;
          }
          lightOn(autoState);
        }
      }
      else {
        autoState = 0;
      }
    }
  }
  delay(100);
}

//0 for off
//light has values 1 2 3 for red green yellow
void lightOn(int light){
  switch(light){
    case 1: //red
      currentColor = "red";
      Serial.println("RED ON");
      digitalWrite(red, HIGH);
      digitalWrite(green, LOW);
      digitalWrite(yellow, LOW);
    break;
    case 2: //green
      currentColor = "green";
      Serial.println("GREEN ON");
      digitalWrite(red, LOW);
      digitalWrite(green, HIGH);
      digitalWrite(yellow, LOW);
    break;
    case 3: //yellow
      currentColor = "yellow";
      Serial.println("YELLOW ON");
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(yellow, HIGH);
    break;
    default: //off
      currentColor = "black";
      Serial.println("OFF");
      digitalWrite(red, LOW);
      digitalWrite(green, LOW);
      digitalWrite(yellow, LOW);
    break;
  }
}

void printHTML(WiFiClient client){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("");
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");

  // The concept and code of the sliding button on this page came from https://www.w3schools.com/howto/howto_css_switch.asp
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  client.println("<style>");
  client.println("@keyframes bordercolor {0% {border-color: red;} 55% {border-color: green;} 100% {border-color: yellow;}}");
  client.println("h1 {text-align: center; width: 100%;}");
  client.printf("div {padding: 5px; border: 5px dashed %s; width: 60%%; margin-left: 15%%; display: flex; flex-direction: column; justify-content: center; align-items: center;}\n", currentColor);
  if(currentColor == "blue"){
    client.println("div {animation: bordercolor 3s infinite !important;}");
  }
  client.println("label {margin: auto; width: 100%; height: 25px; text-align: left !important;}");
  client.println("input {float: right}");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");

  client.println("<h1>Arduino Wifi Stoplight</h1>");

  // This section modifies the page HTML based on the current state of the LED
  //RED
  client.print("<div><label style=\"border-bottom: 1.5px solid red;\">Red<input type=\"radio\" ");
  if (selectedState == "red") {
    client.print("checked ");
  }
  client.print("onclick=\"window.location.href='/red");
  client.println("'\"></label><br/>");

  //YELLOW
  client.print("<label style=\"border-bottom: 1.5px solid yellow;\">Yellow<input type=\"radio\" ");
  if (selectedState == "yellow") {
    client.print("checked ");
  }
  client.print("onclick=\"window.location.href='/yellow");
  client.println("'\"></label><br/>");

  //GREEN
  client.print("<label style=\"border-bottom: 1.5px solid green;\">Green<input type=\"radio\" ");
  if (selectedState == "green") {
    client.print("checked ");
  }
  client.print("onclick=\"window.location.href='/green");
  client.println("'\"></label><br/>");

  
  //AUTO
  client.print("<label style=\"border-bottom: 1.5px solid blue;\">Auto<input type=\"radio\" ");
  if (selectedState == "auto") {
    client.print("checked ");
  }
  client.print("onclick=\"window.location.href='/auto");
  client.println("'\"></label><br/>");

  
  //OFF
  client.print("<label>Off<input type=\"radio\" ");
  if (selectedState == "off") {
    client.print("checked ");
  }
  client.print("onclick=\"window.location.href='/off");
  client.println("'\"></label><br/></div>");

  
  client.println("</body>");
  client.println("</html>");
}
