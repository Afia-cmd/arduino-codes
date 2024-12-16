#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <UrlEncode.h>
#include <DHT.h>

// --- Wi-Fi Credentials ---
const char* ssid = "HUAWEI_B612_DAE9";
const char* password = "D1GM99832HT";

// --- Twilio Credentials ---
const char* account_sid = "AC1af870abfdd5ebc3f35a7ed1dec0f218";
const char* auth_token = "7b358d11f27594052ba3630aed70a3ff";
const char* twilio_whatsapp_number = "whatsapp:14155238886"; // Twilio Sandbox WhatsApp number
const char* farmer_whatsapp_number = "whatsapp:233509886745"; // Farmer's WhatsApp number

// --- Twilio SMS Credentials ---
const char* twilio_sms_number = "18777804236"; // Your Twilio SMS-capable phone number
const char* farmer_sms_number = "+18142472832"; // Farmer's phone number in E.164 format

// --- DHT11 Setup ---
#define DHTPIN 5          // GPIO5 on ESP8266 (D1 pin)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Twilio API Host ---
const char* twilio_api_host = "api.twilio.com";

// --- SSL Fingerprint (Optional but Recommended) ---
// const char* fingerprint = "XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX XX"; // Replace with Twilio's SSL fingerprint

// Function to predict rain based on humidity
bool willItRain(float humidity) {
  const float RAIN_THRESHOLD = 80.0; // Define your own threshold
  return humidity > RAIN_THRESHOLD;
}

// Function to send WhatsApp message via Twilio
void sendWhatsAppMessage(const char* message) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected!");
    return;
  }

  // Create secure WiFi client
  WiFiClientSecure client;
Serial.println(WiFi.localIP()); // Make sure this prints a valid IP address.

  // Optionally, set fingerprint to verify SSL certificate
  // Uncomment the following line if you have the fingerprint
  // client.setFingerprint(fingerprint);

  // If you don't set the fingerprint, you can skip SSL verification (NOT recommended for production)
  client.setInsecure(); // WARNING: This disables SSL certificate verification

  // Initialize HTTP client
  HTTPClient http;

  // Construct the full URL for WhatsApp
  char url[200];
  snprintf(url, sizeof(url), "https://%s/2010-04-01/Accounts/%s/Messages.json", twilio_api_host, account_sid);

  // Begin HTTP connection with the secure client and URL
  if (http.begin(client, url)) { // Updated begin method with WiFiClientSecure
    // Set Basic Auth (Twilio Account SID and Auth Token)
    http.setAuthorization(account_sid, auth_token);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare the POST data for WhatsApp
    char postData[300];
    snprintf(postData, sizeof(postData), "From=%s&To=%s&Body=%s", twilio_whatsapp_number, farmer_whatsapp_number, message);

    // Send the POST request
    int httpCode = http.POST(postData);

    // Check the response
    if (httpCode > 0) {
      Serial.printf("Twilio WhatsApp Response Code: %d\n", httpCode);
      
      /*// Optionally, read the response without using String
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
        int len = http.getSize();
        if(len > 0){
          char payload[len + 1];
          int readLen = http.getStream().readBytes(payload, len);
          payload[readLen] = '\0';
          Serial.println(payload);
        }
      }*/
      // Print the response payload
     String payload = http.getString();
     Serial.println("Response Payload:");
     Serial.println(payload);  // This will show the actual error message from Twilio
    }
    else {
      Serial.printf("Twilio WhatsApp POST failed, error code: %d\n", httpCode);
    }

    // End the HTTP connection
    http.end();
  }
  else {
    Serial.println("Unable to connect to Twilio WhatsApp API");
  }
}

// Function to send SMS message via Twilio
void sendSMSMessage(const char* message) {
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

  // Construct the full URL for SMS
  char url[200];
  snprintf(url, sizeof(url), "https://%s/2010-04-01/Accounts/%s/Messages.json", twilio_api_host, account_sid);

  // Begin HTTP connection with the secure client and URL
  if (http.begin(client, url)) { // Updated begin method with WiFiClientSecure
    // Set Basic Auth (Twilio Account SID and Auth Token)
    http.setAuthorization(account_sid, auth_token);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Prepare the POST data for SMS
    char postData[300];
    snprintf(postData, sizeof(postData), "From=%s&To=%s&Body=%s", twilio_sms_number, farmer_sms_number, message);

    // Send the POST request
    int httpCode = http.POST(postData);

    // Check the response
    if (httpCode > 0) {
      Serial.printf("Twilio SMS Response Code: %d\n", httpCode);
      
      // Optionally, read the response without using String
      /*if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
        int len = http.getSize();
        if(len > 0){
          char payload[len + 1];
          int readLen = http.getStream().readBytes(payload, len);
          payload[readLen] = '\0';
          Serial.println(payload);
        }
      }*/
      String payload = http.getString();
      Serial.println("Response Payload:");
      Serial.println(payload);  // This will show the actual error message from Twilio
    }
    else {
      Serial.printf("Twilio SMS POST failed, error code: %d\n", httpCode);
    }

    // End the HTTP connection
    http.end();
  }
  else {
    Serial.println("Unable to connect to Twilio SMS API");
  }
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

  Serial.println();
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

  // Predict rain
  bool rain = willItRain(humidity);

  // Prepare the messages
  char message_en[300];
  char message_ewe[300];

  if (rain) {
    // English message
    snprintf(message_en, sizeof(message_en),
             "Good morning, Farmer!\nToday's Weather:\nIt is likely to rain today.\nPlease ensure your crops are protected.");

    // Ewe message
    snprintf(message_ewe, sizeof(message_ewe),
             "Ŋdi na wo, Ɖewohawo!\nŊkekeɖiɖi ƒe ŋkeke:\nEle be ame sia ƒe dɔ kpli nu wòna nu vɛ.\nWòmele yewo dzi yi ame sia ƒe dɔ kpli nu.");
  }
  else {
    // English message
    snprintf(message_en, sizeof(message_en),
             "Good morning, Farmer!\nToday's Weather:\nIt is unlikely to rain today.\nYou can proceed with your farming activities.");

    // Ewe message
    snprintf(message_ewe, sizeof(message_ewe),
             "Ŋdi na wo, Ɖewohawo!\nŊkekeɖiɖi ƒe ŋkeke:\nEle be ame sia ƒe dɔ kpli nu nye wòna nu.\nWòmele yewo dzi le wo ƒomeviwo.");
  }

  // Send WhatsApp messages
  sendWhatsAppMessage(message_en);
  delay(1000); // Small delay to ensure messages are sent separately
  sendWhatsAppMessage(message_ewe);

  // Send SMS messages
  sendSMSMessage(message_en);
  delay(1000); // Small delay to ensure messages are sent separately
  sendSMSMessage(message_ewe);

  // Wait 24 hours before sending the next update
  // For testing purposes, you might want to reduce this delay
  delay(86400000); // 24 hours in milliseconds
}



