#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

uint8_t builtInLEDPin = 2 ; // Built-in LED pin on ESP32
bool builtInLEDStatus = LOW; // Initially OFF

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16 columns, 2 rows

void setup() {
  Serial.begin(115200);
  pinMode(builtInLEDPin, OUTPUT); // Set built-in LED pin as output
  digitalWrite(builtInLEDPin, builtInLEDStatus); // Ensure LED is initially OFF

  // LCD initialization
  lcd.begin();
  lcd.backlight();
  lcd.clear();

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.on("/ledon", handle_ledon);
  server.on("/ledoff", handle_ledoff);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

void handle_OnConnect() {
  Serial.println("Built-in LED Status: OFF");
  server.send(200, "text/html", SendHTML(builtInLEDStatus)); 
}

void handle_ledon() {
  builtInLEDStatus = HIGH;
  digitalWrite(builtInLEDPin, builtInLEDStatus); // Turn LED on
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LED ON");
  Serial.println("Built-in LED Status: ON");
  server.send(200, "text/html", SendHTML(true)); 
}

void handle_ledoff() {
  builtInLEDStatus = LOW;
  digitalWrite(builtInLEDPin, builtInLEDStatus); // Turn LED off
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LED OFF");
  Serial.println("Built-in LED Status: OFF");
  server.send(200, "text/html", SendHTML(false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(bool ledStatus){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Built-in LED Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Built-in LED Control</h1>\n";
  ptr += "<p>Built-in LED Status: ";
  ptr += (ledStatus) ? "ON" : "OFF";
  ptr += "</p>";
  if(ledStatus)
    ptr +="<a class=\"button button-off\" href=\"/ledoff\">Turn Off</a>\n";
  else
    ptr +="<a class=\"button button-on\" href=\"/ledon\">Turn On</a>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}


