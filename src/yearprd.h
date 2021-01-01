#ifndef YEARPRD_H
#define YEARPRD_H

#include <Arduino.h>

class YearPrd {
public:
  class DateStr {
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

  YearPrd(time_t t) {
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

  int count;
  int updateStatus;
  void update() {
    updateStatus = 0;

    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("Error on WiFi connection");
      connectWifi();
      delay(500);
    }

    Serial.print("get contribution history `");
    Serial.print(prd());
    Serial.print("`: ");

    if (WiFi.status() == WL_CONNECTED) {

      std::unique_ptr<BearSSL::WiFiClientSecure> client(
          new BearSSL::WiFiClientSecure);
      client->setFingerprint(fingerPrint);

      HTTPClient http;
      http.begin(*client, serverName.c_str());

      int httpCode = http.POST(requestBody());

      if (httpCode > 0) {
        String payload = http.getString();
        JSONVar obj = JSON.parse(payload);
        if (obj.hasOwnProperty("data")) {
          count = (int)obj["data"]["viewer"]["contributionsCollection"]
                          ["contributionCalendar"]["totalContributions"];
          Serial.println("Success");
          updateStatus = 1;
        } else {
          Serial.println("Error on Eval response");
          updateStatus = -1;
        }
      } else {
        Serial.println("Error on HTTP request");
        updateStatus = -2;
      }

      http.end();
    } else {
      Serial.println("Unknown WiFi Error");
      updateStatus = -3;
    }
    return;
  };
};

#endif