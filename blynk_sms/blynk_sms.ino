// Blynk credentials
#define BLYNK_TEMPLATE_ID "TMPL2KQkEiI5t"
#define BLYNK_TEMPLATE_NAME "wineaffair"
#define BLYNK_AUTH_TOKEN "NpUnQrpPP_58TpaZpx-xwa-mvSsyIhLf"

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager


// Twilio credentials
const char *account_sid = "AC86611c1810d842cd2966b36bdfed1daf";
const char *auth_token = "542ca19849fc2f63a764f0e97806db78";
const char *from_number = "+12254389966";
const char *to_number = "+233509581056";

// OpenWeatherMap API credentials
const char* apiKey = "5505e4995e00f1755c61da65f6e74231";
const char* cityID = "2294768"; // You can find your city ID on OpenWeatherMap

// Define the pins
const int rainSensorAnalogPin = 36; // GPIO36 (ADC1_CHANNEL_0)
const int rainSensorDigitalPin = 34; // Digital pin connected to DO
const int dhtPin = 4; // Pin connected to DHT11 data pin
const int ldrPin = 39; // GPIO39 (ADC1_CHANNEL_3)
const int ledPin = 2; // Built-in LED

// DHT11 sensor setup
#define DHTTYPE DHT11
DHT dht(dhtPin, DHTTYPE);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2); // Address 0x27, 16 columns, 2 rows

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  
  // Initialize the DHT sensor
  dht.begin();

  // Set the rain sensor digital pin as input
  pinMode(rainSensorDigitalPin, INPUT);

  // Set the LED pin as output
  pinMode(ledPin, OUTPUT);

  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");

  // Initialize WiFi using WiFiManager
  WiFiManager wifiManager;
  wifiManager.autoConnect("AutoConnectAP");

  // Initialize Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, WiFi.SSID().c_str(), WiFi.psk().c_str());

  lcd.clear();
  lcd.print("Setup complete");
  delay(2000);
  lcd.clear();
}

void loop() {
  // Run Blynk
  Blynk.run();

  // Read the digital value from the rain sensor
  int rainDigitalValue = digitalRead(rainSensorDigitalPin);

  // Read the analog value from the rain sensor
  int rainAnalogValue = analogRead(rainSensorAnalogPin);

  // Read temperature and humidity from DHT11
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Read the analog value from the LDR
  int ldrValue = analogRead(ldrPin);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Determine light intensity
  String lightCondition = (ldrValue > 2000) ? "Light is intense" : "Light is not intense"; // Adjust threshold as needed

  // Print the values to the serial monitor
  Serial.print("Rain Sensor Analog Value: ");
  Serial.println(rainAnalogValue);

  Serial.print("Rain Sensor Digital Value: ");
  Serial.println(rainDigitalValue);

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  Serial.println(lightCondition);

  // Send values to Blynk
  Blynk.virtualWrite(V0, temperature);
  Blynk.virtualWrite(V1, humidity);
  Blynk.virtualWrite(V2, ldrValue);

  // If rain is detected (digital output is low), turn on the LED
  bool isRaining = (rainDigitalValue == LOW);
  if (isRaining) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Rain detected! Weather Condition: Rainy");
  } else {
    digitalWrite(ledPin, LOW);
    Serial.println("No rain detected. Weather Condition: Dry");
  }

  // Fetch and display pressure and wind speed data from OpenWeatherMap
  float pressure = 0.0;
  float windSpeed = 0.0;
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    WiFiClient client;
    String url = String("http://api.openweathermap.org/data/2.5/weather?id=") + cityID + "&appid=" + apiKey;
    http.begin(client,url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      pressure = doc["main"]["pressure"];
      windSpeed = doc["wind"]["speed"];
      Serial.print("Pressure: ");
      Serial.print(pressure);
      Serial.println(" hPa");

      Serial.print("Wind Speed: ");
      Serial.print(windSpeed);
      Serial.println(" m/s");

      // Send pressure and wind speed to Blynk
      Blynk.virtualWrite(V4, pressure);
      Blynk.virtualWrite(V3, windSpeed); // Send wind speed to Virtual Pin V3
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }

  // Display temperature, pressure, and wind speed on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");
  
  lcd.setCursor(0, 1);
  lcd.print("Pres: ");
  lcd.print(pressure);
  lcd.print("hPa");

  lcd.setCursor(8, 1);
  lcd.print("Wnd: ");
  lcd.print(windSpeed);
  lcd.print("m/s");

  // Get the current time
  time_t now = time(nullptr);
  struct tm* timeInfo = localtime(&now);

  // Check if it is 5 am or 5 pm
  if ((timeInfo->tm_hour == 5 && timeInfo->tm_min == 0) || (timeInfo->tm_hour == 17 && timeInfo->tm_min == 0)) {
    // Construct the message based on weather conditions
    String weatherMessage = "";
    if (isRaining) {
      weatherMessage = "It is going to rain today, do not apply fertiliser.";
    } else if (windSpeed > 5.0) { // Adjust wind speed threshold as needed
      weatherMessage = "It is going to be windy, do not apply fertiliser.";
    } else {
      weatherMessage = "The weather is okay for fertiliser application.";
    }

    // Send SMS using Twilio API
    HTTPClient http;
    WiFiClient client;
    String url = "https://api.twilio.com/2010-04-01/Accounts/" + String(account_sid) + "/Messages.json";
    http.begin(client,url);
    http.addHeader("Authorization", "Basic " + String(base64::encode(String(account_sid) + ":" + String(auth_token))));
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String body = "From=" + String(from_number) + "&To=" + String(to_number) + "&Body=" + weatherMessage;
    int httpCode = http.POST(body);

    if (httpCode > 0) {
      Serial.println("Message sent successfully");
    } else {
      Serial.println("Error sending message");
    }

    http.end();

    // Delay for a minute to avoid sending multiple messages in the same minute
    delay(60000);
  }

  // Delay for a bit to avoid flooding the serial monitor
  delay(2000); // Delay for 2 seconds
}
