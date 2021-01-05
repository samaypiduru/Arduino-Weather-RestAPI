#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

const char *SSID = "wifi_ssid";
const char *PWD = "wifi_password";

StaticJsonDocument<250> jsonDocument;
char buffer[250];

const char DHT_TYPE = DHT11;  // uses the dht11 sensor
const int DHT_PIN = 17;
DHT dht(DHT_PIN, DHT_TYPE);

WebServer server(3000);

void beginWiFi() {
  WiFi.begin(SSID, PWD);

  Serial.print("Connecting to ");
  Serial.print(SSID);
  Serial.println(".....");

  while(WiFi.status() != WL_CONNECTED) {
    if(WiFi.status() == WL_CONNECT_FAILED || WiFi.status() == WL_NO_SSID_AVAIL) {
      Serial.println("Connection Failed");
      return;
    }
    delay(100);
  }
  Serial.print("Connected to ");
  Serial.println(WiFi.localIP());
}

void apiReqListen() {  // returns the appropriate json object for a request
  server.on("/temperature", getTemp);
  server.on("/humidity", getHumidity);
  server.on("/heatinfo", getHeatInfo);
  server.begin();
}

void getTemp() {  // gets temp from dht11
  newJsonObj("temperature", dht.readTemperature(true), "F");
  Serial.println("Requested Temperature");
  server.send(200, "application/json", buffer);
}

void getHumidity() {  // gets humidity from dht11
  newJsonObj("humidity", dht.readHumidity(), "%");
  Serial.println("Requested Humidity");
  server.send(200, "application/json", buffer);
}

void getHeatInfo() {  // gets humidity and temperature from dht11
  jsonDocument.clear();
  addJsonObj("temperature", dht.readTemperature(true), "F");
  addJsonObj("humidity", dht.readHumidity(), "%");
  serializeJson(jsonDocument, buffer);
  Serial.println("Requested Heat Info");
  server.send(200, "application/json", buffer);
}

void newJsonObj(char *metricName, float measurement, char *unit) {
  jsonDocument.clear();
  jsonDocument["metric"] = metricName;
  jsonDocument["value"] = measurement;
  jsonDocument["unit"] = unit;
  serializeJson(jsonDocument, buffer);
}

void addJsonObj(char *metricName, float measurement, char *unit) {
  JsonObject objData = jsonDocument.createNestedObject();
  objData["metric"] = metricName;
  objData["value"] = measurement;
  objData["unit"] = unit;
}

void loop() {
  server.handleClient();
}

void setup() {
  Serial.begin(115200);
  beginWiFi();
  dht.begin();
  apiReqListen();
}
