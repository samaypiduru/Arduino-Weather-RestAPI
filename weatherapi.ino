#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char *SSID = "wifi_ssid";
const char *PWD = "wifi_pwd";

StaticJsonDocument<250> jsonDocument;
char buffer[250];

const char DHT_TYPE = DHT11;  // uses the dht11 sensor
const int DHT_PIN = 17;
DHT dht(DHT_PIN, DHT_TYPE);

WebServer server(8080);

void connectToWiFi() {
  Serial.print("Connecting to ");
  Serial.print(SSID);
  Serial.println(".....");
  
  WiFi.begin(SSID, PWD);

  while(WiFi.status() != WL_CONNECTED) {
    delay(200);
  }
  Serial.print("Connected to ");
  Serial.println(WiFi.localIP());
}

void api_route() {  // returns the appropriate metric for a request
  server.on("/temperature", getTemp);
  server.on("/humidity", getHumidity);
  server.on("/heatinfo", getHeatInfo);
  server.begin();
}

void getTemp() {  // gets temp from dht11
  Serial.println("Getting Temperature");
  newJsonObj("temperature", dht.readTemperature(true), "F");
  server.send(200, "application/json", buffer);
}

void getHumidity() {  // gets humidity from dht11
  Serial.println("Getting Humidity");
  newJsonObj("humidity", dht.readHumidity(), "%");
  server.send(200, "application/json", buffer);
}

void getHeatInfo() {  // gets humidity and temperature from dht11
  Serial.println("Getting Heat Info");
  jsonDocument.clear();
  addJsonObj("temperature", dht.readTemperature(true), "F");
  addJsonObj("humidity", dht.readHumidity(), "%");
  serializeJson(jsonDocument, buffer);
  server.send(200, "application/json", buffer);
}

void newJsonObj(char *metric, float value, char *unit) {
  jsonDocument.clear();
  jsonDocument["metric"] = metric;
  jsonDocument["value"] = value;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void addJsonObj(char *metric, float value, char *unit) {
  JsonObject obj = jsonDocument.createNestedObject();
  obj["metric"] = metric;
  obj["value"] = value;
  obj["unit"] = unit;
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  connectToWiFi();
  api_route();
}

void loop() {
  server.handleClient();
}
