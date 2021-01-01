#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "pass.h"

void connectWifi() {
  Serial.print("Connecting to ");
  Serial.print(ssid.c_str());
  Serial.print(": ");
  WiFi.begin(ssid.c_str(), password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("*");
  }
  Serial.println(" DONE!");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(500);
}

#endif