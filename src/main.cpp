#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "pass.h"

void setup() {
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
  delay(1000);
}

void loop() {
  Serial.print("Loop, ");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if ((WiFi.status() == WL_CONNECTED)) { // Check the current connection status

    HTTPClient http;

    http.begin("http://192.168.2.104:3004/posts/1");
    int httpCode = http.GET(); // Make the request

    if (httpCode > 0) { // Check for the returning code

      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println(payload);
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end(); // Free the resources
  }

  delay(1000);
}
