#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <string>


// --- Replace these with your network credentials ---
const char* ssid = "Afia's Galaxy A32";
const char* password = "wz0801wz";

// --- Twilio credentials ---
const char* account_sid = "AC1af870abfdd5ebc3f35a7ed1dec0f218";
const char* auth_token = "7b358d11f27594052ba3630aed70a3ff";
const char* twilio_whatsapp_number = "whatsapp:14155238886"; // Twilio Sandbox WhatsApp number
const char* farmer_whatsapp_number = "whatsapp:233509886745"; // Farmer's WhatsApp number

// --- DHT11 setup ---
#define DHTPIN 4          // GPIO5 on ESP8266 (D1 pin). Use GPIO number instead of D1 to avoid 'D1' not declared error.
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Twilio API Host ---
const char* twilio_api_host = "api.twilio.com";

// --- SSL Fingerprint (Optional but Recommended) ---
// To ensure secure communication, you can use the SSL fingerprint of Twilio's API.
// However, obtaining and updating fingerprints can be cumbersome.
// For testing purposes, SSL verification can be skipped (NOT recommended for production).

//const char* fingerprint = "XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX"; // Replace with Twilio's SSL fingerprint

// Function to send WhatsApp message via Twilio
void sendWhatsAppMessage(String message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return;
  }

  // Create secure WiFi client
  WiFiClientSecure client;

  // Optionally, set fingerprint to verify SSL certificate
  // Uncomment the following line if you have the fingerprint
  // client.setFingerprint(fingerprint);

  // If you don't set the fingerprint, you can skip SSL verification (NOT recommended for production)
  client.setInsecure(); // WARNING: This disables SSL certificate verification

  // Initialize HTTP client
  HTTPClient http;

  // Construct the full URL
  String url = "https://" + String(twilio_api_host) + "/2010-04-01/Accounts/" + String(account_sid) + "/Messages.json";

  // Begin HTTP connection with the secure client and URL
  if (http.begin(client, url)) { // Updated begin method with WiFiClientSecure
    // Set Basic Auth (Twilio Account SID and Auth Token)
    http.setAuthorization(account_sid, auth_token);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare the POST data
    String postData = "From=" + String(twilio_whatsapp_number) +
                      "&To=" + String(farmer_whatsapp_number) +
                      "&Body=" + message;

    // Send the POST request
    int httpCode = http.POST(postData);

    // Check the response
    if (httpCode > 0) {
      Serial.printf("Twilio Response Code: %d\n", httpCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.printf("Twilio POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    // End the HTTP connection
    http.end();
  }
  else {
    Serial.println("Unable to connect to Twilio API");
  }
}

// Function to determine if it will rain based on humidity
bool willRain(float humidity) {
  const float RAIN_THRESHOLD = 80.0; // Define your threshold (adjust as needed)
  return (humidity > RAIN_THRESHOLD);
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Initialize DHT11
  dht.begin();

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
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Read temperature as Celsius (the default)
  float temperature = dht.readTemperature();

  // Read humidity
  float humidity = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Determine if it will rain
  bool rain = willRain(humidity);

  // Prepare messages in three languages
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
  }
  else {
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

  // Send the message
  sendWhatsAppMessage(message);

  // Wait 24 hours before sending the next update
  // For testing purposes, you might want to reduce this delay
  delay(86400000); // 24 hours in milliseconds
}
