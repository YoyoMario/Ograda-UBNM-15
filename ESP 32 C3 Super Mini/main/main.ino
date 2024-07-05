//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;

#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "ZTE_5ACDBF";
const char* password = "6WSU5JYD6E";

const uint8_t RELAY_PIN = 4;

// Create a WebServer object
WebServer server(8086);

void handleRoot() {
  server.send(200, "text/plain", "Hello random stranger! :)");
}

void handleSezameOtvoriSe() {
  server.send(200, "text/plain", "Susedi, ide gas!");

  delay(10);

  // Simulate relay press.
  digitalWrite(RELAY_PIN, HIGH);
  delay(200);
  digitalWrite(RELAY_PIN, LOW);
}

void handleTest() {
  server.send(200, "text/plain", "This is a test page.");
}

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  
  // Set WiFi to stay active
  WiFi.setSleep(false);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/", handleRoot);
  server.on("/sezameOtvoriSe", handleSezameOtvoriSe);
  server.on("/test", handleTest);

  // Start the server
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  // Handle client requests
  server.handleClient();
  delay(10);
}
