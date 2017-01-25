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
    Node(const char* backendUrl, int tempPin, int humPin);
    String init();
    float retrieveTemp();
    void postTemp(float temp);
    void runServer();
    bool isConnected;
  private:
    struct Response {
      const char* status;
      const char* error;
    };
    
    struct SetupRequest {
      const char* ssid;
      const char* pass;
    };
    struct Config {
      const char* ssid;
      const char* pass;
    };
    OneWire _oneWire;
    DallasTemperature _sensors;
    const char* BACKEND_URL;
    const char* SSID;
    const char* PASSWORD;
    String _macId;
    int _tempPin;
    int _humPin;
    String _getMacId();
    void _getConfig(Config& config);
    void _setConfig(Config& newConfig);
    void _handleSetup();
    int _connectToWifi(const char*, const char*);
    bool _deserializeSetupRequest(SetupRequest& reqBody, String& json);
    void _serializeResponse(Response& res, String& json);
};

#endif
