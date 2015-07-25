#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#inlcude "wifipass.h"

// add in a file called wifipass.h in /src/ or define here: 
//const char* ssid = "yourssid";
//const char* password = "password";

//void* __dso_handle;

ESP8266WebServer server(80);
 
const int led = 13;

int adc_value;
char strbuffer[7]; 

 
void handle_root() {
  digitalWrite(led, 0);
  delay(100);
  adc_value=analogRead(A0);
  //digitalWrite(led, 1);
  String valueString = String(adc_value);
  server.send(200, "text/plain", "hello from esp8266!"+valueString);
  delay(100);
  
}
 
void setup(void)
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    digitalWrite(led, 0);
    Serial.print(".");
    delay(500);
    digitalWrite(led, 1);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  server.on("/", handle_root);
  
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void)
{
  server.handleClient();
} 
