#ifndef TIME_H
#define TIME_H

#include <Arduino.h>
#include <ESP8266WiFi.h>

void getJST() {
  const int JST = 3600 * 9;
  time_t t = 0;

  Serial.print("Get JST: ");
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

  int i = 0;
  while (t < 160937596 && i <= 30) {
    delay(500);
    Serial.print("*");
    t = time(null);
    i++;
  }

  Serial.println(" DONE!");

  Serial.print("Now : ");
  Serial.println(t);
  delay(500);
}

#endif