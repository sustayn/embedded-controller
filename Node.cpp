/*
  Node.h - Library for using the ESP as a node for recording sensor data.
  Created by Jake Dluhy, January 21, 2017.
*/

#include "Arduino.h"
#include "Node.h"

#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ArduinoJson.h"

#include "OneWire.h"
#include "DallasTemperature.h"

#include "EEPROM.h"

StaticJsonBuffer<200> jsonBuffer;
HTTPClient http;

const char* SSID = "Bill Wi the Science Fi";
const char* PASSWORD = "Bill!Bill!Bill!Bill!";

Node::Node(char* backendUrl, int tempPin, int humPin)
{
  OneWire oneWire(tempPin);
  DallasTemperature _sensors(&oneWire);
  _backendUrl = backendUrl;
  _tempPin = tempPin;
  _humPin = humPin;
}

String Node::init()
{
  WiFi.begin(SSID, PASSWORD);
  _sensors.begin();

  int i = 0;
  while(WiFi.status() != WL_CONNECTED && i < 10) {
    delay(500);
    i++;
  }

  if(i == 10) {
    return String("Unable to connect to WiFi");
  } else {
    return String("Successfully connected to WiFi");
  }
}

float retrieveTemp()
{
  _sensors.requestTemperatures();
  float temp = _sensors.getTempCByIndex(0);
  return temp;
}

void postTemp(float temp) {
  JsonObject& payload = jsonBuffer.createObject();
  payload["value"] = temp;

  char stringified[256];
  payload.printTo(stringified);

  http.begin(_backendUrl + "/api/v1/nodes/" + "1" + "/temperatures");
  http.addHeader("Content-Type", "application/json");
  http.POST(stringified);
  http.end();
}
