#include <PubSubClient.h>
#include <WiFiManager.h>

#define LED  2
#define DOOR_SENSOR 13

int currentDoorState = 0;
int lastDoorState = 0;
const char* mqttServer = "77.170.41.205";
const int mqttPort = 1883;
const char* mqttUser = "chinou";
const char* mqttPassword = "277353";
const char* mqttTopic = "mqtt/esp32";

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
    Serial.begin(115200);
    pinMode(LED, OUTPUT);
    pinMode(DOOR_SENSOR, INPUT_PULLUP);
  
    WiFiManager manager;    
    bool success = manager.autoConnect("ESP32_AP","password");
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(LED, HIGH);
      delay(200);
      digitalWrite(LED, LOW);
    }

    if(!success) {
        Serial.println("Failed to connect");
    } else {
        digitalWrite(LED, HIGH);
        delay(1000);
        digitalWrite(LED, LOW);
        Serial.println("WiFi Established");
        client.setServer(mqttServer, mqttPort);
        while (!client.connected()) {
          Serial.println("Connecting to MQTT...");
          if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
            Serial.println("MQTT Connection Established");
            client.publish(mqttTopic, "Connected!");
          } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
         }       
       }  
    }
} 

void loop() {
  client.loop();
  lastDoorState = currentDoorState; 
  currentDoorState = digitalRead(DOOR_SENSOR);

  if (lastDoorState == LOW && currentDoorState == HIGH) {
    Serial.println("Deur is geopend!");
    client.publish(mqttTopic, "OPEND");
    delay(500);

  } else if (lastDoorState == HIGH && currentDoorState == LOW) {
    Serial.println("Door is gesloten!");
    client.publish(mqttTopic, "CLOSED");
    delay(500);
  }
}