#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <vector>
#include "Arduino_JSON.h"
#include "pass.h"

const int JST = 3600 * 9;
long int threeDays = 60 * 60 * 24 * 3;
long int oneYear = 60 * 60 * 24 * 365;
time_t t;
std::vector<String> datePeriod;

class dateToStrClass {
  struct tm *tm;
  String year;
  String mon;
  String mday;

public:
  void setTime(time_t t) {
    tm = localtime(&t);
    year = String(tm->tm_year + 1900);
    if (tm->tm_mon + 1 < 10) {
      mon = String("0" + String(tm->tm_mon + 1));
    } else {
      mon = String(tm->tm_mon + 1);
    }
    if (tm->tm_mday < 10) {
      mday = String("0" + String(tm->tm_mday));
    } else {
      mday = String(tm->tm_mday);
    }
  };
  String dateStr() {
    return String("\\\"" + year + "-" + mon + "-" + mday +
                  "T00:00:00+09:00\\\"");
  };
} from, to;

void setDatePeriod(time_t t) {
  struct tm *tm;
  tm = localtime(&t);
  datePeriod.clear();
  while ((tm->tm_year + 1900) > 2015) {
    from.setTime(t + threeDays - oneYear);
    to.setTime(t + threeDays);
    datePeriod.push_back(
        String("from: " + from.dateStr() + ", to: " + to.dateStr()));

    t -= oneYear;
    tm = localtime(&t);
  }
}

int getContribution(String prd) {
  int counter = 0;
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(
        new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerPrint);

    HTTPClient http;
    http.begin(*client, serverName.c_str());

    String query =
        String("{\"query\": \"query {viewer {contributionsCollection(" + prd +
               ") {contributionCalendar {totalContributions}}}}\"}");
    int httpCode = http.POST(query);
    Serial.println();
    Serial.print("query :");
    Serial.println(query);
    if (httpCode > 0) {
      String payload = http.getString();
      JSONVar obj = JSON.parse(payload);
      if (obj.hasOwnProperty("data")) {
        counter = (int)obj["data"]["viewer"]["contributionsCollection"]
                          ["contributionCalendar"]["totalContributions"];
      } else {
        Serial.print("Error on Eval response");
      }
    } else {
      Serial.print("Error on HTTP request");
    }

    http.end();
  } else {
    Serial.println("Error on WiFi connection");
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
  return counter;
}
void getHistory() {
  for (String prd : datePeriod) {

    Serial.print("History: ");
    Serial.println(getContribution(prd));
  }
}
void setup() {
  Serial.begin(115200);
  delay(3000);

  // wifi setup
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

  // Time setup
  Serial.print("GetGlobalTime: ");
  configTime(JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");
  while (time(&t) < 160937596) {
    delay(500);
    Serial.print("*");
  }
  Serial.println(" DONE!");

  Serial.print("Now : ");
  Serial.println(t);

  setDatePeriod(t);
  getHistory();
  delay(500);
}

void loop() {
  if ((WiFi.status() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(
        new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerPrint);

    HTTPClient http;
    http.begin(*client, serverName.c_str());

    String query =
        String("{\"query\": \"query {viewer {contributionsCollection(from: "
               "\\\"2020-05-16T11:25:30+09:00\\\", to: "
               "\\\"2021-05-16T11:25:30+09:00\\\") {contributionCalendar "
               "{totalContributions}}}}\"}");
    int httpCode = http.POST(query);
    // int httpCode =
    //     http.POST("{\"query\": \"query {viewer
    //     {contributionsCollection(from: "
    //               "\\\"2020-05-16T11:25:30+09:00\\\", to: "
    //               "\\\"2021-05-16T11:25:30+09:00\\\") "
    //               "{contributionCalendar {totalContributions}}}}\"}");
    Serial.println();
    Serial.print("query :");
    Serial.println(query);
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
      Serial.print(", system timer: ");
      Serial.print(time(&t));
      Serial.print(", total contributions: ");
      Serial.println(counter);
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }

  delay(1000);
}
