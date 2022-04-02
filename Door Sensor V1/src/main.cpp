#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "VM HQ";
const char* password = "##############";
char JsonOuput[128];
int counter = 0;

String apiurl = "http://192.168.1.22/api/qhaQGX7srfXZw-1oK3O0-X4CXHRr93jioCo2SL7P/groups/1/action";

#define DOOR_SENSOR_PIN 15
#define ONBOARD_LED  2

int currentDoorState = 0;
int lastDoorState = 0;

void setup() {
  Serial.begin(115200);  
  pinMode(DOOR_SENSOR_PIN, INPUT_PULLUP);
  pinMode(ONBOARD_LED,OUTPUT);

  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(ONBOARD_LED,HIGH);
    delay(500);
    digitalWrite(ONBOARD_LED,LOW);
  }
  digitalWrite(ONBOARD_LED,HIGH);
  delay(2000);
  digitalWrite(ONBOARD_LED,LOW);
}

void loop() {
  lastDoorState = currentDoorState; 
  currentDoorState = digitalRead(DOOR_SENSOR_PIN);

  if (lastDoorState == LOW && currentDoorState == HIGH) { // state change: LOW -> HIGH
    counter++;
    Serial.println(counter);
    if(WiFi.status()== WL_CONNECTED) {
      HTTPClient http;
      
      http.begin(apiurl);
      http.addHeader("Content-Type", "application/json");

      const size_t CAPACITY = JSON_OBJECT_SIZE(3);
      StaticJsonDocument<CAPACITY> doc;
      
      JsonObject obj = doc.to<JsonObject>();
      obj["on"] = true;

      serializeJson(doc, JsonOuput);

      int httpResponseCode = http.PUT(String(JsonOuput));
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);

        http.end();
      }
    }
    delay(500);
  }

  if (lastDoorState == HIGH && currentDoorState == LOW) { // state change: HIGH -> LOW
    counter++;
    Serial.println(counter);
    if(WiFi.status()== WL_CONNECTED && counter % 4 == 0){
      HTTPClient http;
      
      http.begin(apiurl);
      http.addHeader("Content-Type", "application/json");

      const size_t CAPACITY = JSON_OBJECT_SIZE(3);
      StaticJsonDocument<CAPACITY> doc;
      
      JsonObject obj = doc.to<JsonObject>();
      obj["on"] = false;

      serializeJson(doc, JsonOuput);

      int httpResponseCode = http.PUT(String(JsonOuput));
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);

        http.end();
      }
    }
  }
  delay(500);
}