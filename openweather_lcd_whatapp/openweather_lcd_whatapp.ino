#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// --- Replace these with your network credentials ---
const char* ssid = "Afia's Galaxy A32";
const char* password = "wz0801wz";

// --- OpenWeatherMap API details ---
// OpenWeatherMap API credentials
const char *apiKey = "5505e4995e00f1755c61da65f6e74231";
const char *city = "2294768"; // You can find your city ID on OpenWeatherMap
const char* owm_host = "api.openweathermap.org";

// --- Twilio credentials ---
const char* account_sid = "AC1af870abfdd5ebc3f35a7ed1dec0f218";
const char* auth_token = "7b358d11f27594052ba3630aed70a3ff";
const char* twilio_whatsapp_number = "whatsapp:14155238886"; 
const char* farmer_whatsapp_number = "whatsapp:233509886745";

// --- DHT11 setup ---
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- LCD setup (20x4 I2C) ---
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Function to fetch weather data from OpenWeatherMap
void fetchWeatherData(float &windspeed, float &pressure) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return;
  }

  HTTPClient http;
  WiFiClient client;
  String url = String("http://") + owm_host + "/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";
  
  http.begin(client,url);
  int httpCode = http.GET();

  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
    
    // Parse the JSON to get windspeed and pressure
    int windIndex = payload.indexOf("\"speed\":") + 8;
    int pressureIndex = payload.indexOf("\"pressure\":") + 11;
    
    windspeed = payload.substring(windIndex, payload.indexOf(",", windIndex)).toFloat();
    pressure = payload.substring(pressureIndex, payload.indexOf(",", pressureIndex)).toFloat();
  } else {
    Serial.println("Failed to retrieve weather data.");
  }
  
  http.end();
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Display welcome message on LCD
  lcd.setCursor(0, 0);
  lcd.print("Weather Station");
  delay(2000); // Delay for 2 seconds
  lcd.clear();
}

void loop() {
  delay(2000);  // Wait 2 seconds between readings

  // Read temperature and humidity from DHT11
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  // Check if the DHT11 read failed
  if (isnan(temperature) || isnan(humidity)) {
    lcd.setCursor(0, 0);
    lcd.print("DHT11 Error!");
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Fetch windspeed and pressure from OpenWeatherMap
  float windspeed = 0.0, pressure = 0.0;
  fetchWeatherData(windspeed, pressure);

  // Display weather data on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temp: " + String(temperature, 1) + "C");
  
  lcd.setCursor(0, 1);
  lcd.print("Humidity: " + String(humidity, 1) + "%");

  lcd.setCursor(0, 2);
  lcd.print("Windspeed: " + String(windspeed, 1) + " m/s");

  lcd.setCursor(0, 3);
  lcd.print("Pressure: " + String(pressure, 1) + " hPa");

  // Determine if it will rain and send WhatsApp message
  bool rain = willRain(humidity);
  String message = prepareMessage(rain);
 // sendWhatsAppMessage(message);

  delay(60000);  // Wait for a minute before updating the display
}

// Function to determine if it will rain based on humidity
bool willRain(float humidity) {
  const float RAIN_THRESHOLD = 80.0;
  return (humidity > RAIN_THRESHOLD);
}

// Function to prepare message in English, Twi, and Ewe
String prepareMessage(bool rain) {
  String message = "";

  if (rain) {
    // English
    message += "English:\n";
    message += "Good morning Farmer!\n";
    message += "It's expected to rain today. Please prepare accordingly.\n\n";

    // Twi
    message += "Twi:\n";
    message += "Mema wo akye, ɔhene!\n";
    message += "Ɛbɛsɛeɔ adeɛ da yi. Mesrɛ wo sɛ hyɛ nkuran mu.\n\n";

    // Ewe
    message += "Ewe:\n";
    message += "Mawuena ŋutɔ, ŋkeke!\n";
    message += "Egbɔ bɔxɔ̃ le xexeme me. Mìele be woate ŋutiɖa yi.\n";
  } else {
    // English
    message += "English:\n";
    message += "Good morning Farmer!\n";
    message += "No rain is expected today. It's a good day to work on your crops.\n\n";

    // Twi
    message += "Twi:\n";
    message += "Mema wo akye, ɔhene!\n";
    message += "Da yi, nsuo nni ho. Ɛyɛ da pa sɛ wubetumi adwuma wɔ wo mmoa so.\n\n";

    // Ewe
    message += "Ewe:\n";
    message += "Mawuena ŋutɔ, ŋkeke!\n";
    message += "Nɔ ƒe bɔxɔ̃ nɔa le xexeme me. Eŋutiɖa gɔ̃ la, wòate ŋutiɖa yi ŋutɔ.\n";
  }

  return message;
}
