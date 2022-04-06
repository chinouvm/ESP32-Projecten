#include <WiFi.h>
#include <PubSubClient.h>

#define DOOR_SENSOR_PIN  13  // ESP32 pin GIOP19 connected to door sensor's pin
int currentDoorState; // current state of door sensor
int lastDoorState;    // previous state of door sensor
 
const char* ssid = "VM HQ";
const char* password = "LoekiIsEenHond#10";
const char* mqttServer = "192.168.1.69";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP); // set ESP32 pin to input pull-up mode
  currentDoorState = digitalRead(DOOR_SENSOR_PIN); // read state of door sensor
 
  Serial.print("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("."); 
  } 
  Serial.println("\nConnected to the WiFi network!!"); 

  client.setServer(mqttServer, mqttPort);
 
  Serial.print("\nConnecting to MQTT");
  while (!client.connected()) {
      Serial.print(".");
    if (client.connect("ESP32MQTTCLIENT")) {
      Serial.println("\nMQTT Connected! :D");  
    } else {
      Serial.print("failed with state: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  	
  client.publish("mqtt/deur", "Hello from ESP32");
}

void loop() {
  client.loop();
  lastDoorState = currentDoorState;              // save the last state
  currentDoorState  = digitalRead(DOOR_SENSOR_PIN); // read new state

  if (lastDoorState == LOW && currentDoorState == HIGH) { // state change: LOW -> HIGH DEUR OPEN
    client.publish("mqtt/deur", "OPEN");
  }
  else
  if (lastDoorState == HIGH && currentDoorState == LOW) { // state change: HIGH -> LOW DEUR DICHT
    client.publish("mqtt/deur", "DICHT");
  }
}