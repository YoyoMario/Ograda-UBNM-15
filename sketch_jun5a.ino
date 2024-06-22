#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

//static const uint8_t D0   = 16;
//static const uint8_t D1   = 5;
//static const uint8_t D2   = 4;
//static const uint8_t D3   = 0;
//static const uint8_t D4   = 2;
//static const uint8_t D5   = 14;
//static const uint8_t D6   = 12;
//static const uint8_t D7   = 13;
//static const uint8_t D8   = 15;

const char* SSID = "ZTE_5ACDBF";
const char* PASSWORD = "6WSU5JYD6E";

const int FLASH_COUNT = 5;
const int FLASH_DURATION = 50;

const uint8_t LED_PIN = 16; 
const uint8_t RELAY_PIN = 14;

const char* serverName = "http://192.168.0.5:8086/test";
WiFiClient client;
int currentIterationCount = 0;
int timeToWait = 30; // 30 seconds.

ESP8266WebServer server(8086);

void setup()
{
  // Start the Serial communication to send messages to the computer.
  Serial.begin(115200);

  // Define pin functionality.
  pinMode(LED_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Default turn off LED and relay.
  toggleRelayPin(LOW);
  toggleLEDPin(HIGH);

  delay(2000);

  ConnectToWifi();

  // Enable watchdog timer
  ESP.wdtEnable(WDTO_8S);
}

void loop() 
{
  if(WiFi.status() == WL_CONNECTED)
  {
    // Handle client.
    server.handleClient();
    SentTestGetRequest();
  }

  delay(1000); 
}

void ConnectToWifi()
{
  // Visual signal connection has started.
  toggleLEDPin(LOW);

  //WiFi.mode (WIFI_STA);

    // Disable persistent storage for WiFi credentials
  WiFi.persistent(false);

  // Disable sleep modes
  wifi_set_sleep_type(NONE_SLEEP_T);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  
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

  // Assign listening path.
  server.on("/sezameOtvoriSe", handleRoot);
  server.on("/test", handleTest);
  server.on("/watchdog", watchdogTimer);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  // Visual signal can be turned off since we've completed.
  toggleLEDPin(HIGH);
}

void SentTestGetRequest()
{
  currentIterationCount ++;
  if(currentIterationCount < timeToWait)
  {    
    //Serial.println("Too early...");
    return;
  }

  //Serial.println("Sending!");
  currentIterationCount = 0; // Reset.

  HTTPClient http;
  http.begin(client, serverName); // Specify the URL
  http.GET(); // Send the GET request
  http.end(); // End the request

}

void handleRoot() 
{
  // Respond immediatelly.
  server.send(200, "text/plain", "Susedi, ide gas!");
  // Simulate relay press.
  toggleRelayPin(HIGH);
  // Flash the led so we have visual feedback as well.
  flashLED(FLASH_COUNT, FLASH_DURATION);
  // Simulate relay release.
  toggleRelayPin(LOW);
}

void handleTest()
{  
  // Respond immediatelly.
  server.send(200, "text/plain", "Test successfull!");
}

void watchdogTimer()
{
  server.send(200, "text/plain", "Free heap: " + String(ESP.getFreeHeap()));
  // Reset the watchdog timer
  ESP.wdtFeed();
}

void handleNotFound()
{
  server.send(404, "text/plain", "Not found");
}

void flashLED(int flashCount, int delayValue)
{
  for(int i = 0; i < flashCount; i++)
  {
    toggleLEDPin(LOW);
    delay(delayValue);
    toggleLEDPin(HIGH);
    delay(delayValue);
  }
}

void toggleRelayPin(uint8 value)
{
  digitalWrite(RELAY_PIN, value);
}

void toggleLEDPin(uint8 value)
{
  digitalWrite(LED_PIN, value);
}
