#include "Node.h"

#define TEMP_PIN 2

const char* BACKEND_URL = "http://192.168.0.103:3000";

Node node(BACKEND_URL, TEMP_PIN, 0);

void setup() {
  Serial.begin(115200);
  delay(100);

  String response = node.init();
  Serial.println(response);
}

void loop() {
  if(node.isConnected) {
    delay(2000);

    float temp = node.retrieveTemp();
    Serial.print("Temp: ");
    Serial.println(temp);
  
//    node.postTemp(temp);
  } else {
    node.runServer();
  }
}
