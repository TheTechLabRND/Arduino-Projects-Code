  
/*
  Motion Trigger
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

// Type your WiFi name (SSID) and password here:
const char* ssid = "your_ssid_here";
const char* password = "your_password_here";

const int httpsPort = 443;
const char* host = "maker.ifttt.com";

// Replace with your eventname and api key - open the documentation: https://ifttt.com/maker_webhooks
String event_name = "motion_triggered";
String api_key = "your_api_key_here";
String url = "/trigger/" + event_name + "/with/key/" + api_key;

// Main setup - this part of the code runs only once on startup
void setup() {
  // Begin serial connection, mostly for debugging
  Serial.begin(9600);
  // Initialize the on-board LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // turn off the LED (HIGH is OFF for the on board LED)

  // Initialize WiFi and print on serial monitor (for debugging)
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  makeHTTPRequest();
  // Sleep until a PIR motion is detected
  ESP.deepSleep(0);
}

void loop() {

}
void makeHTTPRequest() {
  //Send an HTTP POST request when PIR is triggered
  if (WiFi.status() == WL_CONNECTED) {      // check if WiFi is connected
    digitalWrite(LED_BUILTIN, LOW);         //turn on the LED
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    WiFiClientSecure client;    // create a new HTTP client
    Serial.print("connecting to ");
    Serial.println(host);

    if (!client.connect(host, httpsPort)) {   //connect to HTTP client, show error if failed to connect.
      Serial.println("Error: Connection failed.");
      return;
    }

    Serial.print("requesting URL: ");
    Serial.println(url);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +             // request to HTTP client, in order to trigger IFTTT webhooks service.
                 "Host: " + host + "\r\n" +
                 "User-Agent: BuildFailureDetectorESP32\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("request sent");

    // get the reply from HTTP and print on serial monitor
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }

    String line = client.readStringUntil('\n');

    Serial.println("reply was:");
    Serial.println("==========");
    Serial.println(line);
    Serial.println("==========");
    Serial.println("closing connection");
    Serial.println("");

    delay(10000);   // long delay to avoid too many triggers at once

    digitalWrite(LED_BUILTIN, HIGH);   //turn the LED off as there's no more motion
  } else {
    // If the device fails to connect to WiFi, print an error
    Serial.println("Error: WiFi not connected.");
  }
}
