#include <Arduino.h>

#include <ESP8266WiFi.h>
#include "pass.h"

void setup() {
  pinMode(1, OUTPUT);
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  // wifi setup
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    WiFi.localIP();
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void loop() {}
