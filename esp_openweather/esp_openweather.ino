#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "Afia's Galaxy A32";
const char* password = "wz0801wz";
const char* apiKey = "5505e4995e00f1755c61da65f6e74231";
const char* cityID = "2294768"; // Example city ID

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected!");

  // Create WiFiClient and HTTPClient objects
  WiFiClient client;
  HTTPClient http;

  // Make the request to OpenWeatherMap
  String url = "http://api.openweathermap.org/data/2.5/weather?id=" + String(cityID) + "&appid=" + String(apiKey);
  http.begin(client, url);

  int httpCode = http.GET();

  if (httpCode > 0) {
    // HTTP request successful
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error on HTTP request: ");
    Serial.println(httpCode);
  }

  http.end();
}

void loop() {
  // Empty loop
}
