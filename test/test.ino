#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

// Replace with your network credentials
const char* ssid = "ZTE_5ACDBF";
const char* password = "6WSU5JYD6E";

// Create an instance of the WiFiServer class
WiFiServer server(8087);

const char* url = "http://httpbin.org/get";   // host address of HTTPBin
HTTPClient http;   
WiFiClient client;

void sendHttpRequest() {

    http.begin(client, url);
    int httpCode = http.GET();
   // String payload = http.getString();
    http.end();
    
    Serial.print("HTTP Code:");
    Serial.println(httpCode);
}

void connectToWiFi(){
  WiFi.mode(WIFI_STA);
  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void handleRoot(WiFiClient& client) {
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\nHello World!<br>";
  s += "Click <a href=\"/on\">here</a> to turn on the LED.<br>";
  s += "Click <a href=\"/off\">here</a> to turn off the LED.<br>";
  s += "</html>\r\n";
  client.print(s);
}

void handleLEDOn(WiFiClient& client) {
  digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\nLED is ON<br>";
  s += "Click <a href=\"/off\">here</a> to turn off the LED.<br>";
  s += "</html>\r\n";
  client.print(s);
}

void handleLEDOff(WiFiClient& client) {
  digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\nLED is OFF<br>";
  s += "Click <a href=\"/on\">here</a> to turn on the LED.<br>";
  s += "</html>\r\n";
  client.print(s);
}

void handleTest(WiFiClient& client) {
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\nLED is OFF<br>";
  s += "Test success!";
  s += "</html>\r\n";
  client.print(s);
}

void handleNotFound(WiFiClient& client) {
  String s = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n404 Not Found<br>";
  s += "</html>\r\n";
  client.print(s);
}

void setup() {
  Serial.begin(115200);

  // Connect to Wifi()
  connectToWiFi();
    
  WiFi.setSleepMode(WIFI_NONE_SLEEP);

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.println(WiFi.localIP());

  // Set GPIO pins
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW); // Initialize the LED to be off
}

unsigned long previousMillis = 0;      // will store last time request was made
const long interval = 10000;           // interval at which to make requests (30 seconds)

void loop() {
unsigned long currentMillis = millis();
  // Check if it's time to send the next request
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    sendHttpRequest();
  }

  // Check if the WiFi connection is lost and reconnect if necessary
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
    return;
  }


  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("New Client.");
  String currentLine = "";
  String request = "";
  bool currentLineIsBlank = true;

  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      request += c;
      if (c == '\n' && currentLineIsBlank) {
        // End of headers, handle the request
        Serial.println("Request: " + request);

        if (request.indexOf("GET / ") >= 0) {
          handleRoot(client);
        } else if (request.indexOf("GET /on ") >= 0) {
          handleLEDOn(client);
        } else if (request.indexOf("GET /off ") >= 0) {
          handleLEDOff(client); 
        } else if (request.indexOf("GET /test ") >= 0) {
          handleTest(client);
        } else {
          handleNotFound(client);
        }

        break;
      }
      if (c == '\n') {
        currentLineIsBlank = true;
      } else if (c != '\r') {
        currentLineIsBlank = false;
      }
    }
  }

  delay(1); // Give the web browser time to receive the data
  client.stop();
  Serial.println("Client Disconnected.");
}