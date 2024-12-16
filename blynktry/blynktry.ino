#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Blynk credentials from Blynk Web Dashboard
#define BLYNK_TEMPLATE_ID "TMPL2tUG6jMjq"
#define BLYNK_TEMPLATE_NAME "wineaffair"
#define BLYNK_PRINT Serial
#define BLYNK_AUTH_TOKEN "MwkM--7tTldIPQ_qQbfSTHj65Fue8umY"

// Wi-Fi credentials
char ssid[] = "HUAWEI_B612_DAE9";
char pass[] = "D1GM99832HT";

BlynkTimer timer;

// This function is called every time the Virtual Pin 0 state changes
BLYNK_WRITE(V0)
{
  // Set incoming value from pin V0 to a variable
  int value = param.asInt();

  // Update state
  Blynk.virtualWrite(V1, value);
}

// This function is called every time the device is connected to the Blynk.Cloud
BLYNK_CONNECTED()

// This function sends Arduino's uptime every second to Virtual Pin 2.
void myTimerEvent()
{
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Blynk.virtualWrite(V2, millis() / 1000);
}



void setup() {
    Serial.begin(115200);
  
    // Connect to Blynk using the Auth Token
    Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
    Serial.println("Connected to Blynk Web");
    timer.setInterval(1000L, myTimerEvent);
}
}

void loop() {
    // Run Blynk to maintain the connection
    Blynk.run();
    timer.run();
}
