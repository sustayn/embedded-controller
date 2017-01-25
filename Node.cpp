/*
  Node.h - Library for using the ESP as a node for recording sensor data.
  Created by Jake Dluhy, January 21, 2017.
*/

#include "Arduino.h"
#include "Node.h"

#include "ArduinoJson.h"

#include "ESP8266WiFi.h"
#include "WiFiClient.h"
#include "ESP8266WebServer.h"
#include "ESP8266HTTPClient.h"

#include "OneWire.h"
#include "DallasTemperature.h"

#include "EEPROM.h"

const int EEPROM_SIZE = 512;
const int WIFI_CONNECT_TIMEOUT = 20 * 1000;

//StaticJsonBuffer<EEPROM_SIZE> jsonBuffer;
HTTPClient http;
ESP8266WebServer server(80);

Node::Node(const char* backendUrl, int tempPin, int humPin) :
  _oneWire(tempPin),
  _sensors(&_oneWire)
{
  BACKEND_URL = backendUrl;
  _tempPin = tempPin;
  _humPin = humPin;
  _macId = _getMacId();
}

/**
 * Initialize the node and associated libraries. Meant to be run in the setup of the application
 * @return A message describing the status of initialization
 */
String Node::init()
{
  Config config;
  _getConfig(config);

  SSID = config.ssid;
  PASSWORD = config.pass;

  _sensors.begin();

  if(SSID && PASSWORD) {
    WiFi.begin(SSID, PASSWORD);

    isConnected = true; // TODO: Make sure it's connected?
    return String("Found ssid and password");
  } else {
    isConnected = false;

    char apSSID[10];
    String nodeName = "Node_" + _macId.substring(0, 4);
    nodeName.toCharArray(apSSID, sizeof(apSSID));
    
    WiFi.softAP(apSSID);
    IPAddress myIP = WiFi.softAPIP();

    server.on("/setup", HTTP_POST, [this]() { _handleSetup(); });
    server.begin();
    
    return String("Setting up access point with IP: " + myIP.toString());
  }
}

float Node::retrieveTemp()
{
  _sensors.requestTemperatures();
  float temp = _sensors.getTempCByIndex(0);
  return temp;
}

void Node::postTemp(float temp) {
//  JsonObject& payload = jsonBuffer.createObject();
//  payload["value"] = temp;
//
//  char stringified[256];
//  payload.printTo(stringified);
//
//  http.begin(String(BACKEND_URL) + "/api/v1/nodes/" + "1" + "/temperatures");
//  http.addHeader("Content-Type", "application/json");
//  http.POST(stringified);
//  http.writeToStream(&Serial);
//  http.end();
}

void Node::runServer() {
  server.handleClient();
}

/**
 * PRIVATE
 */

/**
 * Get the 12 digit mac ID created by converting the 6 byte mac address to hex and concatenating into a string
 * @return The macId
 */
String Node::_getMacId() {
  byte mac[6];
  WiFi.macAddress(mac);

  String macId = String(mac[5], HEX);
  for(int i = 4; i >= 0; i--) {
    macId += String(mac[i], HEX);
  }

  macId.toUpperCase();
  return macId;
}

void Node::_handleSetup() {
  SetupRequest reqBody;
  Response res;

  String plain = server.arg("plain");
  _deserializeSetupRequest(reqBody, plain);
  int httpCode = 200;

  const char* ssid = reqBody.ssid;
  const char* pass = reqBody.pass;

  if(ssid && pass) {
    int status = _connectToWifi(ssid, pass);

    if(status > 0) {
      Config config;
      _getConfig(config);

      config.ssid = ssid;
      config.pass = pass;

      _setConfig(config);

      res.status = "success";
      res.error = NULL;
    } else {
      res.status = "error";
      res.error = "unable to connect to wifi";
      httpCode = 422;
    }
  } else {
    res.status = "error";
    res.error = "missing ssid or password";
    httpCode = 422;
  }

  String stringified;
  _serializeResponse(res, stringified);
  server.send(httpCode, "application/json", stringified);

  if(res.status == "success") {
    WiFi.softAPdisconnect(true);
  }
}

bool Node::_deserializeSetupRequest(SetupRequest& reqBody, String& json) {
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(json);

  reqBody.ssid = root["ssid"];
  reqBody.pass = root["pass"];

  return root.success();
}

void Node::_serializeResponse(Response& res, String& json) {
  StaticJsonBuffer<256> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();

  if(res.status) root["status"] = res.status;
  if(res.error) root["error"] = res.error;

  root.printTo(json);
}


int Node::_connectToWifi(const char* ssid, const char* password) {
  int timeoutCounter = 0;
  int delayPeriod = 500;
  int totalTimeoutCount = WIFI_CONNECT_TIMEOUT / delayPeriod;

  WiFi.begin(SSID, PASSWORD);
  while(WiFi.status() != WL_CONNECTED && timeoutCounter < totalTimeoutCount) {
    delay(delayPeriod);
    timeoutCounter++;
  }

  if(timeoutCounter == totalTimeoutCount) {
    Serial.println("Unable to connect to Wifi");
    return -1;
  } else {
    Serial.println("Connected to WiFi");
    isConnected = true;
    return 1;
  }
}

// REFACTOR OUT
/**
 * Get the stored config from EEPROM
 * @return The stored config as a JsonObject
 */
void Node::_getConfig(Config& config) {
  StaticJsonBuffer<EEPROM_SIZE> jsonBuffer;
  char stored[EEPROM_SIZE];
  byte value;

  EEPROM.begin(EEPROM_SIZE);
  for(int i = 0; i < EEPROM_SIZE; i++) {
    value = EEPROM.read(i);
    if(value != 0 && value != 255) {
      stored[i] = value;
    }
  }

  JsonObject& configObj = jsonBuffer.parseObject(stored);
  config.ssid = configObj["ssid"];
  config.pass = configObj["pass"];

//  return jsonBuffer.parseObject(stored);
}

void Node::_setConfig(Config& newConfig) {
  StaticJsonBuffer<EEPROM_SIZE> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  char stringified[EEPROM_SIZE];

  root["ssid"] = newConfig.ssid;
  root["pass"] = newConfig.pass;
  root.printTo(stringified);

  EEPROM.begin(EEPROM_SIZE);
  for(int i = 0; i < sizeof(stringified) - 1; i++) {
    if((int)stringified[i] != 0) {
      EEPROM.write(i, (int)stringified[i]);
    }
  }
  EEPROM.commit();
}
// END REFACTOR OUT
