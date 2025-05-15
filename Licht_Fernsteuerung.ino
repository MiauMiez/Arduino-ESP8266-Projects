//Alexander Scholz, May 15 2025 - Verbindet eine ESP Schaltung mit dem Router und hostet einen Webserver, der ermöglicht im gleichen lokalen Netzwerk die LED's zu steuern. 
#include <ESP8266WiFi.h>    
#include <ESP8266WebServer.h>  

#define led_red 16
#define led_green 5 
#define led_blue 4  
const int buttonPin = 2; 
int buttonState = 0 ;
bool light_state; 

//Lokales Netzwerk ohne Internetanbindng  
const char* ssid = "Elektrolabor_no_Internet";
const char* pwd =  "topsecret"; 
ESP8266WebServer server(80);

void setup() {
 
  pinMode(led_red, OUTPUT);
  pinMode(led_green, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(buttonPin, INPUT);

  //Netzwerk 
  Serial.begin(115200); 
  WiFi.begin(ssid, pwd); 
  while (WiFi.status() != WL_CONNECTED) { 
    delay(1000);
    digitalWrite(led_red, HIGH);
  }

  //LED leuchtet rot bis eine Verbindung steht 
  digitalWrite(led_red, LOW);
  digitalWrite(led_green, HIGH);
  delay(1000); 
  digitalWrite(led_green, LOW);

  server.on("/", []() {
  server.send(200, "text/html",
    "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1.0'>"
    "<style>"
    "body { font-family: Arial; text-align: center; margin-top: 50px; }"
    "h1 { font-size: 2em; margin-bottom: 40px; }"
    ".button {"
    "  display: inline-block;"
    "  padding: 20px 40px;"
    "  margin: 10px;"
    "  font-size: 1.5em;"
    "  color: white;"
    "  background-color: #007BFF;"
    "  border: none;"
    "  border-radius: 10px;"
    "  text-decoration: none;"
    "}"
    ".off { background-color: #dc3545; }"
    "</style></head><body>"
    "<h1>LED Steuerung</h1>"
    "<a class='button' href='/on'>LED AN</a><br>"
    "<a class='button off' href='/off'>LED AUS</a>"
    "</body></html>"
  );
});

  server.on("/on", []() {
       light_state = true; 
       server.sendHeader("Location", "/");
       server.send(303); 
    });

  server.on("/off", []() {
      light_state = false; 
      server.sendHeader("Location", "/");
      server.send(303);
    });
    
  server.begin();
}
   
void loop() {

    server.handleClient();

    if (light_state) {
          digitalWrite(led_blue, HIGH);
      } else {
          digitalWrite(led_blue, LOW);
    }
}
