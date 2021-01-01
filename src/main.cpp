#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>
#include <vector>
#include "Arduino_JSON.h"
#include "pass.h"
#include "wifi.h"
#include "time.h"

class yearPrd {
public:
  class dateStr {
  private:
    struct tm *tm;
    String year;
    String mon;
    String mday;

  public:
    void setDate(time_t t) {
      tm = localtime(&t);
      year = String(tm->tm_year + 1900);

      mon = String(tm->tm_mon + 1);
      if (tm->tm_mon + 1 < 10) { // 0埋め
        mon = "0" + mon;
      }

      mday = String(tm->tm_mday);
      if (tm->tm_mday < 10) { // 0埋め
        mday = "0" + mday;
      }
    };

    String str() {
      return String("\\\"" + year + "-" + mon + "-" + mday +
                    "T00:00:00+09:00\\\"");
    };

    String date() { return String(year + "-" + mon + "-" + mday); };
  } from, to;

  static const long int threeDays = 60 * 60 * 24 * 3;
  static const long int oneYear = 60 * 60 * 24 * 365;

  yearPrd(time_t t) {
    from.setDate(t + threeDays - oneYear);
    to.setDate(t + threeDays);
  };

  String str() { return String("from: " + from.str() + ", to: " + to.str()); };

  String prd() {
    return String("from: " + from.date() + ", to: " + to.date());
  };

  String requestBody() {
    return String("{\"query\": \"query {viewer {contributionsCollection(" +
                  str() + ") {contributionCalendar {totalContributions}}}}\"}");
  };
};

std::vector<yearPrd> yearPeriods;
int yearContributions = 0;
int totalContributions = 0;

void setYearPeriods(time_t t) {
  struct tm *tm;
  tm = localtime(&t);
  yearPeriods.clear();
  while ((tm->tm_year + 1900) > 2015) {
    yearPrd year(t);
    yearPeriods.push_back(t);
    t -= yearPrd::oneYear;
    tm = localtime(&t);
  }
}

int getContribution(yearPrd yearPeriod) {
  int response = 0;

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Error on WiFi connection");
    connectWifi();
    delay(500);
  }

  Serial.print("get contribution history `");
  Serial.print(yearPeriod.prd());
  Serial.print("`: ");

  if (WiFi.status() == WL_CONNECTED) {

    std::unique_ptr<BearSSL::WiFiClientSecure> client(
        new BearSSL::WiFiClientSecure);
    client->setFingerprint(fingerPrint);

    HTTPClient http;
    http.begin(*client, serverName.c_str());

    int httpCode = http.POST(yearPeriod.requestBody());

    if (httpCode > 0) {
      String payload = http.getString();
      JSONVar obj = JSON.parse(payload);
      if (obj.hasOwnProperty("data")) {
        yearContributions =
            (int)obj["data"]["viewer"]["contributionsCollection"]
                    ["contributionCalendar"]["totalContributions"];
        Serial.println("Success");
        response = 1;
      } else {
        Serial.println("Error on Eval response");
        response = -1;
      }
    } else {
      Serial.println("Error on HTTP request");
      response = -2;
    }

    http.end();
  } else {
    Serial.println("Unknown WiFi Error");
    response = -3;
  }

  return response;
}

void getHistory() {
  for (uint i = 1; i < yearPeriods.size(); i++) {
    if (getContribution(yearPeriods[i]) >= 0) {
      totalContributions += yearContributions;
      Serial.print("contribution: ");
      Serial.println(yearContributions);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(3000);

  // wifi setup
  connectWifi();

  // Time setup
  time_t t = getJST();
  setYearPeriods(t);
  getHistory();

  delay(500);
}

void loop() {
  if (getContribution(yearPeriods[0]) >= 0) {
    Serial.print("total contribution: ");
    Serial.println(totalContributions + yearContributions);
  } else {
    Serial.println("Error");
  }
}
