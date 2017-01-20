#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* SSID = "Bill Wi the Science Fi";
const char* PASSWORD = "Bill!Bill!Bill!Bill!";

void setup() {
  Serial.begin(115200);
  delay(100);

  logWifiConnect();
  WiFi.begin(SSID, PASSWORD);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  logWifiSuccess();
}

void loop() {
  delay(5000);

  String postResponse = postTemp(1, random(30));
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
