#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include "Arduino_JSON.h"
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
  // configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
  delay(1000);
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(
        new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerPrint);

    HTTPClient http;
    http.begin(*client, serverName);

    int httpCode =
        http.POST("{\"query\": \"query {viewer {contributionsCollection(from: "
                  "\\\"2020-05-16T11:25:30+09:00\\\", to: "
                  "\\\"2021-05-16T11:25:30+09:00\\\") "
                  "{contributionCalendar {totalContributions}}}}\"}");

    if (httpCode > 0) {
      int counter = 0;
      String payload = http.getString();
      JSONVar obj = JSON.parse(payload);
      if (obj.hasOwnProperty("data")) {
        counter = (int)obj["data"]["viewer"]["contributionsCollection"]
                          ["contributionCalendar"]["totalContributions"];
      }

      Serial.print("http code: ");
      Serial.print(httpCode);
      Serial.print(", total contributions: ");
      Serial.println(counter);
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }

  delay(1000);
}
