#ifndef TIME_H
#define TIME_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

time_t getJST() {
  const int JST = 3600 * 9;
  time_t t;

  Serial.print("Get JST: ");
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

  int i = 0;
  while (time(&t) < 160937596 && i <= 30) {
    delay(500);
    Serial.print("*");
    i++;
  }

  Serial.println(" DONE!");

  Serial.print("Now : ");
  Serial.println(t);
  delay(500);
  return t;
}

#endif