#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <PulseSensorPlayground.h> // Install this library for PulseSensor

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Server details
const char* serverAddress = "YOUR_SERVER_ADDRESS";
const int serverPort = 80; // Replace with your server port if different

// Pulse Sensor object
PulseSensorPlayground pulseSensor;

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Initialize the Pulse Sensor
  pulseSensor.begin();
}

void loop() {
  // Check for new heart rate data
  if (pulseSensor.sawNewSample()) {
    int heartRate = pulseSensor.getBeatsPerMinute();
    Serial.print("Heart Rate: ");
    Serial.println(heartRate);

    // Send heart rate data to the server
    sendDataToServer(heartRate);
  }
}

void sendDataToServer(int heartRate) {
  // Use the Wi-Fi client to communicate with the server
  WiFiClient client;
  if (client.connect(serverAddress, serverPort)) {
    Serial.println("Connected to server");

    // Prepare the data to be sent
    String data = "heart_rate=" + String(heartRate);

    // Make a POST request to the server
    client.println("POST /path/to/endpoint HTTP/1.1");
    client.println("Host: " + String(serverAddress));
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(data.length());
    client.println();
    client.println(data);

    // Wait for the server's response (optional)
    while (client.connected() && !client.available()) {
      delay(10);
    }
    if (client.available()) {
      Serial.println("Server Response:");
      while (client.available()) {
        Serial.write(client.read());
      }
    }

    // Close the connection
    client.stop();
    Serial.println("\nConnection closed");
  } else {
    Serial.println("Failed to connect to server");
  }
}