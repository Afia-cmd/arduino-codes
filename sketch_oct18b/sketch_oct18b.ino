#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <DHT.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal_I2C.h>

// OpenWeatherMap API credentials
const char* apiKey = "5505e4995e00f1755c61da65f6e74231";
const char* cityID = "2294768";

// Define the pins
#define DHTPIN 4  // DHT11 pin
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Initialize LCD (20x4 display)

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Initialize WiFiManager
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect("WeatherMonitoringAP")) {
    Serial.println("Failed to connect and hit timeout");
    
    // Display "Not Connected" on the LCD
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Not Connected");
    
    return;  // Return here instead of restarting the device
  }

  Serial.println("Connected to WiFi");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected to WiFi");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  float pressure = 0.0;
  float windSpeed = 0.0;

  // Declare WiFiClient
  WiFiClient client;

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String("http://api.openweathermap.org/data/2.5/weather?id=") + cityID + "&appid=" + apiKey + "&units=metric";
    
    http.begin(client, url);
    
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);

      pressure = doc["main"]["pressure"].as<float>();
      windSpeed = doc["wind"]["speed"].as<float>();

      Serial.print("Pressure: ");
      Serial.print(pressure);
      Serial.println(" hPa");

      Serial.print("Wind Speed: ");
      Serial.print(windSpeed);
      Serial.println(" m/s");
    } else {
      Serial.println("Error on HTTP request");
    }

    http.end();
  }

  // Display the weather data on the LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 2);
  lcd.print("Pressure: ");
  lcd.print(pressure);
  lcd.print("hPa");

  lcd.setCursor(0, 3);
  lcd.print("Wind: ");
  lcd.print(windSpeed);
  lcd.print(" m/s");
  
  delay(60000);  // Delay before sending the next data (1 minute)
}
