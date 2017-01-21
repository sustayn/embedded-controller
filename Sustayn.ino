#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* SSID = "Bill Wi the Science Fi";
const char* PASSWORD = "Bill!Bill!Bill!Bill!";

void setup() {
  Serial.begin(115200);
  delay(100);

  logWifiConnect();
  WiFi.begin(SSID, PASSWORD);
  sensors.begin();

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  logWifiSuccess();
}

void loop() {
  delay(2000);

  sensors.requestTemperatures();
  float temp = sensors.getTempCByIndex(0);
  Serial.print("Temp: ");
  Serial.println(temp);

  getPing();
  String postResponse = postTemp(1, temp);
  Serial.println(postResponse);
}

void logWifiConnect() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);
}

void logWifiSuccess() {
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
