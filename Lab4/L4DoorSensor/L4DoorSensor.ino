#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* wifiSSID = "eesc"; // In order for this to work, you MUST specify the SSID for your wifi
const char* wifiPSK = "eesceesc"; // And the preshared key (wifi password)

const char* mqttServer = "192.168.137.115";
const char* topic = "/lab4/door";
String host = "door";

int door = D1;

WiFiClient wifiClient;
PubSubClient client(wifiClient);

//REFERENCES
//ESP8266 MQTT SETUP
//https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/
//PUBSUBCLIENT MQTT DOCUMENTATION
//https://pubsubclient.knolleary.net/

void setup() { 
 Serial.begin(115200); // Open a serial connection (for debugging)
 delay(10); 
 pinMode(door, INPUT_PULLUP);
 pinMode(LED_BUILTIN, OUTPUT);

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
 client.setServer(mqttServer, 1883);
 client.setCallback(callback);
 while (!client.connected()) {     // Function to call when new messages are received
  client.connect(host.c_str());     // Connect to the broker with unique hostname
  Serial.println(client.state());   // Show debug info about MQTT state
 }
  
 digitalWrite(LED_BUILTIN, LOW);
}
void loop() {
  
  if(digitalRead(door) == HIGH){
    Serial.println("DOOR IS OPEN");
    digitalWrite(LED_BUILTIN, HIGH);
    client.publish(topic, "open");
  }
  else {
    Serial.println("DOOR IS CLOSED");
    digitalWrite(LED_BUILTIN, LOW);
    client.publish(topic, "close");
  }

  delay(1000);
}

void callback(char* topicChar, byte* payload, unsigned int length){
  
}
