#include <ESP8266WiFi.h>

const char* ssid = "HUAWEI_B612_DAE9";
const char* password = "D1GM99832HT";

void setup() {
  // Start serial communication at 115200 baud rate
  Serial.begin(115200);
  delay(10);

  // Connect to WiFi
  Serial.println();
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected.");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Keep it empty for now
}
