//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* SSID = "ZTE_5ACDBF";
const char* PASSWORD = "6WSU5JYD6E";

const int FLASH_COUNT = 5;
const int FLASH_DURATION = 50;

const uint8_t LED_PIN = 16; 
const uint8_t RELAY_PIN = 14;

ESP8266WebServer server(8087);

void handleRoot() {
  String response = "Hello Random Stranger! :)";
  server.send(200, "text/html", response);
}

void handleSezameOtvoriSe() {
  String response = "Susedi, ide gas! ;)";
  server.send(200, "text/html", response);
  delay(10);
  // Simulate relay press.
  toggleRelayPin(HIGH);
  // Flash the led so we have visual feedback as well.
  flashLED(FLASH_COUNT, FLASH_DURATION);
  // Simulate relay release.
  toggleRelayPin(LOW);
}

void handleTest() {
  String response = "Test succeeded!)";
  server.send(200, "text/html", response);
}

void handleNotFound() {
  String s = "<!DOCTYPE HTML>\r\n<html>\r\n404 Not Found<br>";
  s += "</html>\r\n";
  server.send(404, "text/html", s);
}

void ConnectToWifi() {
  // Visual signal connection has started.
  toggleLEDPin(LOW);

  //WiFi.mode (WIFI_STA);

  // Disable persistent storage for WiFi credentials
  WiFi.persistent(false);

  // Disable sleep modes
  // wifi_set_sleep_type(NONE_SLEEP_T);
  // WiFi.setSleepMode(WIFI_NONE_SLEEP);
  
  // Connect to the network.
  WiFi.begin(SSID, PASSWORD);

  // Notify user via message.s
  Serial.print("Connecting to: ");
  Serial.print(SSID); 
  Serial.println("...");

  // Wait for the Wi-Fi to connect and write elapsed time.
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  // Write success messages.
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  
  // Visual signal can be turned off since we've completed.
  toggleLEDPin(HIGH);
}

void SetupServer() {
  // Start the server
  server.begin();
  Serial.println("Server started");
  // Define the routes
  server.on("/", handleRoot);
  server.on("/sezameOtvoriSe", handleSezameOtvoriSe);
  server.on("/test", handleTest);
  server.onNotFound(handleNotFound);
}

void flashLED(int flashCount, int delayValue){
  for(int i = 0; i < flashCount; i++)
  {
    toggleLEDPin(LOW);
    delay(delayValue);
    toggleLEDPin(HIGH);
    delay(delayValue);
  }
}

void toggleRelayPin(uint8 value){
  digitalWrite(RELAY_PIN, value);
}

void toggleLEDPin(uint8 value){
  digitalWrite(LED_PIN, value);
}

void setup() {
  Serial.begin(115200);

  // Define pin functionality.
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Default turn off LED and relay.
  toggleRelayPin(LOW);
  toggleLEDPin(HIGH);

  delay(2000);

  ConnectToWifi();
  SetupServer();

  // Set GPIO pins
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize the LED to be off
}

void loop() {
  // Handle client requests
  server.handleClient();
}