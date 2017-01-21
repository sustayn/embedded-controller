/*
  Node.h - Library for using the ESP as a node for recording sensor data.
  Created by Jake Dluhy, January 21, 2017.
*/

#ifndef Node_h
#define Node_h

#include "Arduino.h"

#include "ESP8266WiFi.h"
#include "ESP8266HTTPClient.h"
#include "ArduinoJson.h"

#include "OneWire.h"
#include "DallasTemperature.h"

#include "EEPROM.h"
 
class Node
{
  public:
    Node(char* backendUrl, int tempPin, int humPin);
    String init();
    float retrieveTemp();
    void postTemp(float temp);
  private:
    DallasTemperature _sensors;
    char* _backendUrl;
    int _tempPin;
    int _humPin;
}

#endif
