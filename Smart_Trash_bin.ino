#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Define the WiFi credentials
#ifndef STASSID
#define STASSID "AndroidAP470E"
#define STAPSK  "nody2004@"
#endif


// Define IR sensor and LED pins
const int irSensorPin = D1;   // Example GPIO pin number for IR sensor
const int ledPin = LED_BUILTIN; // Example GPIO pin number for LED

// Define the initial state of the bin
bool isBinFull = false;

const char* ssid = STASSID;
const char* password = STAPSK;

// TCP server at port 80 will respond to HTTP requests
WiFiServer server(80);

void setup(void) {
  Serial.begin(115200);


  
  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");
}

void loop(void) {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  Serial.println("New client");

  // Wait for data from client to become available
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  
  Serial.print("Request: ");
  Serial.println(req);
  client.flush();

  String s;
  if (req == "/" || req == "/on1" || req == "/on2") {
    // Construct the HTTP response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<html><head><title>Bin Status</title></head><body>");
    client.println("<h1>Bin 123 Status</h1>");
    client.print("<p>This bin is ");
    client.print(isBinFull ? "full" : "not full");
    client.println("</p>");
    client.println("</body></html>");
  }
  
  client.stop();
  Serial.println("Client disconnected");
 
  // Update the bin status based on the IR sensor
  if (digitalRead(irSensorPin) == HIGH) {
    isBinFull = true;
    digitalWrite(ledPin, HIGH);
    Serial.println("Bin 123 is full. I am full.");
  } else {
    isBinFull = false;
    digitalWrite(ledPin, LOW);
    Serial.println("Bin 123 is not full.");
  }

  delay(1000);
}
