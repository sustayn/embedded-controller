const String BASE_URL = "http://192.168.0.101:3000";

String getPing() {
  HTTPClient http;
  // Use WiFiClient class to create TCP connections
  http.begin(BASE_URL + "/ping");

  int httpCode = http.GET();

  if(httpCode < 0) {
    Serial.println("[HTTP] GET failed, error: ");
    String error = http.errorToString(httpCode).c_str();
    Serial.println(error);
    return error;
  }

  Serial.printf("[HTTP] GET code: %d\n", httpCode);

  if(httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    return payload;
  }
}

String postTemp(int nodeId, float temp) {
  HTTPClient http;
  http.begin(BASE_URL + "/api/v1/nodes/" + String(nodeId) + "/temperatures");
  http.addHeader("Content-Type", "application/json");
  http.POST("{\"value\":" + String(temp) + "}");
  http.writeToStream(&Serial);
  http.end();

  return String("hello world");
}
