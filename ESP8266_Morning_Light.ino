#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'

ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80

void handleRoot();              // function prototypes for HTTP handlers
void handleChangeColor();
void handleLightOn();
void handleLightOff();
void handleAlarmOn();
void handleAlarmOff();
void handleNotFound();

const int redPin = 14;     // 13 corresponds to GPIO13
const int greenPin = 12;   // 12 corresponds to GPIO12
const int bluePin = 13;    // 14 corresponds to GPIO14

unsigned long oldTime = 0;
const unsigned long interval = 1000;

int alarmHours = 7;
int alarmMinutes = 0;
int alarmState = 0;
String red = "255";
String green = "255";
String blue = "255";

// Winter Time
#define NTP_OFFSET   60 * 60      // In seconds

// Summer Time
//#define NTP_OFFSET   60 * 60 * 2  // In seconds
#define NTP_INTERVAL 60 * 1000    // In miliseconds
#define NTP_ADDRESS  "europe.pool.ntp.org"

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, NTP_OFFSET, NTP_INTERVAL);

DynamicJsonDocument doc(200);

int animationState = 0;
#define animationInterval (500)
unsigned long lastAnimationUpdate = 0;
int rowFrame = 0;

void setup(void){
  Serial.begin(115200);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');

  analogWrite(redPin, 255);
  analogWrite(greenPin, 255);
  analogWrite(bluePin, 255);

  wifiMulti.addAP("SSID", "PASSWORD"); 

  Serial.println("Connecting ...");
  int i = 0;
  while (wifiMulti.run() != WL_CONNECTED) { // Wait for the Wi-Fi to connect: scan for Wi-Fi networks, and connect to the strongest of the networks above
    delay(250);
    Serial.print('.');
  }
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());               // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer

  server.on("/", HTTP_GET, handleRoot);
  server.on("/changeColor", HTTP_POST, handleChangeColor);
  server.on("/lightOn", HTTP_GET, handleLightOn);
  server.on("/lightOff", HTTP_GET, handleLightOff);
  server.on("/alarmOn", HTTP_POST, handleAlarmOn);
  server.on("/alarmOff", HTTP_GET, handleAlarmOff);
  server.onNotFound(handleNotFound);

  server.begin();                            // Actually start the server
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
  unsigned long currentTime = millis();

  if (currentTime - oldTime >= interval) {
    timeClient.update();
    int hour = timeClient.getHours();
    int minutes = timeClient.getMinutes();

    Serial.println(timeClient.getFormattedTime());

    if (hour == alarmHours && minutes == alarmMinutes && alarmState == 1 && animationState == 0) {
      animationState = 1;
    }

    oldTime = currentTime;
  }

  if (animationState == 1 && (millis() - lastAnimationUpdate > animationInterval)) {
    drawSunriseFrame();
    lastAnimationUpdate = millis();
  }
}

void handleRoot() {
    server.send(200, "text/html", rootHtml());
}

void handleChangeColor() {
  if( !server.hasArg("red") || ! server.hasArg("blue") || server.arg("red") == NULL || server.arg("blue") == NULL) {
    server.send(400, "text/plain", "400: Invalid Request");
    return;
  }
  
  red = server.arg("red");
  green = server.arg("green");
  blue = server.arg("blue");
  String data;
  
  JsonObject root  = doc.to<JsonObject>();
  root["red"] = red;
  root["green"] = green;
  root["blue"] = blue;
  root["message"] = "Color Changed";
  serializeJson(doc, data);

  changeColor(red, green, blue);
  
  server.send(200, "text/json", data);
}

void handleLightOn() {
  turnLightOn();
  String data;
  
  JsonObject root  = doc.to<JsonObject>();
  root["message"] = "Lights on";
  serializeJson(doc, data);
  
  server.send(200, "text/html", data);
}

void handleLightOff() {
  turnLightOff();
  String data;
  
  JsonObject root  = doc.to<JsonObject>();
  root["message"] = "Lights off";
  serializeJson(doc, data);
  
  server.send(200, "text/html", data);
}

void handleAlarmOn() {
  if( !server.hasArg("hours") || ! server.hasArg("minutes") || server.arg("hours") == NULL || server.arg("minutes") == NULL) {
    server.send(400, "text/plain", "400: Invalid Request");
    return;
  }
  
  alarmHours = server.arg("hours").toInt();
  alarmMinutes = server.arg("minutes").toInt();
  alarmState = 1;
  rowFrame = 0;
  
  String data;
  
  JsonObject root  = doc.to<JsonObject>();
  root["message"] = "Alarm set";
  serializeJson(doc, data);
  
  server.send(200, "text/json", data);
}

void handleAlarmOff() {
  alarmState = 0;
  animationState = 0;
  rowFrame = 0;
  String data;
  
  JsonObject root  = doc.to<JsonObject>();
  root["message"] = "Alarm off";
  serializeJson(doc, data);
  
  server.send(200, "text/json", data);
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}

void turnLightOn() {
  analogWrite(redPin, red.toInt());
  analogWrite(greenPin, green.toInt());
  analogWrite(bluePin, blue.toInt());
}

void turnLightOff() {
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
  animationState = 0;
  alarmState = 0;
  rowFrame = 0;
}

void changeColor(String r, String g, String b) {
  analogWrite(redPin, r.toInt());
  analogWrite(greenPin, g.toInt());
  analogWrite(bluePin, b.toInt());
}
