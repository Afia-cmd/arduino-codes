#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <Wire.h>
#include <WiFiManager.h>

// WiFi credentials (used by WiFiManager to connect or create an access point)
char ssid[] = "Afia's Galaxy A32";
char pass[] = "wz0801wz";

// Twilio credentials
const char *account_sid = "AC1af870abfdd5ebc3f35a7ed1dec0f218";
const char *auth_token = "7b358d11f27594052ba3630aed70a3ff";
const char *from_number = "18142472832";
const char *to_number = "233509886745";

// OpenWeatherMap API credentials
const char *apiKey = "5505e4995e00f1755c61da65f6e74231";
const char *cityID = "2294768"; // You can find your city ID on OpenWeatherMap

// Define the pins for sensors
const int dhtPin = 4;               // GPIO 4 connected to DHT11 data pin
//const int ldrPin = A0;              // GPIO A0 for LDR
//onst int ledPin = 2;               // Built-in LED on GPIO 2

// DHT11 sensor setup
#define DHTTYPE DHT11
DHT dht(dhtPin, DHTTYPE);

WiFiClient client; // Create WiFiClient object

void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    delay(1000);  // Wait for a second to establish the connection

    // Initialize the DHT sensor
    dht.begin();

    // Set the LED pin as output
    //pinMode(ledPin, OUTPUT);

    // Initialize WiFi using WiFiManager
    WiFiManager wifiManager;
    if (wifiManager.autoConnect("AutoConnectAP")) {
        Serial.println("Connected to WiFi");
    } else {
        Serial.println("WiFi connection failed");
    }
}

void sendSMS(const String& message) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = "https://api.twilio.com/2010-04-01/Accounts/" + String(account_sid) + "/Messages.json";
        
        http.begin(client, url);  // Pass the client object when calling http.begin
        http.setAuthorization(account_sid, auth_token);

        String postData = "Body=" + message + "&From=" + String(from_number) + "&To=" + String(to_number);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        
        int httpCode = http.POST(postData);
        
        if (httpCode > 0) {
            Serial.print("Twilio Response: ");
            Serial.println(http.getString());
        } else {
            Serial.print("Error on sending POST: ");
            Serial.println(httpCode);
        }
        
        http.end();
    }
}

void loop() {
    // Read temperature and humidity from DHT11
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temperature)) {
        Serial.println("Failed to read from DHT sensor!");
        return;
    }

    // Print the values to the serial monitor
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");

    // Fetch weather data from OpenWeatherMap
    float pressure = 0.0;
    float windSpeed = 0.0;
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String("http://api.openweathermap.org/data/2.5/weather?id=") + cityID + "&appid=" + apiKey;
        http.begin(client, url); // Pass the client object here as well
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
        } else {
            Serial.println("Error on HTTP request");
        }

        http.end();
    }

    // Get the current time
    time_t now = time(nullptr);
    struct tm *timeInfo = localtime(&now);

    // Check if it is 5 am or 5 pm
    if ((timeInfo->tm_hour == 5 && timeInfo->tm_min == 0) || (timeInfo->tm_hour == 17 && timeInfo->tm_min == 0)) {
        // Construct the message based on weather conditions
        String weatherMessage = "";
        if (windSpeed > 5.0) { // Adjust wind speed threshold as needed
            weatherMessage = "It is going to be windy, do not apply fertilizer.";
        } else {
            weatherMessage = "The weather is okay for fertilizer application.";
        }
        
        sendSMS(weatherMessage);

        // Delay for a minute to avoid sending multiple messages in the same minute
        delay(60000);
    }

    // Delay for a bit to avoid flooding the serial monitor
    delay(2000); // Delay for 2 seconds
}
