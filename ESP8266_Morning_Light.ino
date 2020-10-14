#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer server(80);

void handleRoot();
void handleChangeColor();
void handleLightOn();
void handleLightOff();
void handleNotFound();

const int redPin = 13;
const int greenPin = 12;
const int bluePin = 14;

DynamicJsonDocument doc(200);

void setup(void){
	Serial.begin(115200);
	delay(10);
	Serial.println('\n');

  // WIFI CREDENTIALS
	wifiMulti.addAP("SSID", "PASSWORD"); 

	Serial.println("Connecting ...");
	int i = 0;
  // Waiting for Wifi
	while (wifiMulti.run() != WL_CONNECTED) {
		delay(250);
		Serial.print('.');
	}
	Serial.println('\n');
	Serial.print("Connected to ");
	Serial.println(WiFi.SSID());
	Serial.print("IP address:\t");
	Serial.println(WiFi.localIP());

	if (MDNS.begin("esp8266")) {
		Serial.println("mDNS responder started");
	} else {
		Serial.println("Error setting up MDNS responder!");
	}

	server.on("/", HTTP_GET, handleRoot);
	server.on("/changeColor", HTTP_POST, handleChangeColor);
  server.on("/lightOn", HTTP_GET, handleLightOn);
  server.on("/lightOff", HTTP_GET, handleLightOff);
	server.onNotFound(handleNotFound);

	server.begin();
	Serial.println("HTTP server started");
}

void loop(void){
	server.handleClient();
}

void handleRoot() {
	  server.send(200, "text/html", rootHtml());
}

void handleChangeColor() {
	if( !server.hasArg("red") || ! server.hasArg("green") || ! server.hasArg("blue") || server.arg("red") == NULL || server.arg("green") == NULL || server.arg("blue") == NULL) {
		server.send(400, "text/plain", "400: Invalid Request");
		return;
	}
  
	String red = server.arg("red");
  String green = server.arg("green");
  String blue = server.arg("blue");
  String data;
  
  JsonObject root  = doc.to<JsonObject>();
  root["red"] = red;
  root["green"] = green;
  root["blue"] = blue;
  root["message"] = "Color Changed";
  serializeJson(doc, data);
  
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

void handleNotFound(){
	server.send(404, "text/plain", "404: Not found");
}

void turnLightOn() {
  analogWrite(redPin, 255);
  analogWrite(greenPin, 255);
  analogWrite(bluePin, 255);
}

void turnLightOff() {
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);
}

void changeColor(String r, String g, String b) {
  analogWrite(redPin, r.toInt());
  analogWrite(greenPin, g.toInt());
  analogWrite(bluePin, b.toInt());
}
