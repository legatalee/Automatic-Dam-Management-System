#include <WiFiClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <FirebaseArduino.h>
#include <SoftwareSerial.h>

#define FIREBASE_HOST "automatic-dam.firebaseio.com"
#define FIREBASE_AUTH "3rxeL5tbMRMQn7qecowaJumq9XDd7MRH1DS6Hd3l"
#define WIFI_SSID "DimiFi_2G"
#define WIFI_PASSWORD "newdimigo"

SoftwareSerial HC12(D3, D4);

HTTPClient http;

void setup() {
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);

  Serial.begin(115200);
  HC12.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.stream("/queue");
}

String path;
String data;

void loop() {
  String path = "";
  String data = "";
  
  while (HC12.available()) {
    data += (char) HC12.read();
    delay(1);
  }

  if (Firebase.available()) {
    FirebaseObject event = Firebase.readEvent();
    String eventType = event.getString("type");
    eventType.toLowerCase();
    if (eventType == "put") {
      path = event.getString("path");
      data = event.getString("data");
      Serial.print("data: ");
      Serial.println(data);
      Serial.print("data: ");
      Serial.println(path);
    }
  }
  
  if (data == "on1") {
    digitalWrite(D1, HIGH);
    removeData(path);
  }
  if (data == "off1") {
    digitalWrite(D1, LOW);
    removeData(path);
  }
  if (data == "on2") {
    digitalWrite(D2, HIGH);
    removeData(path);
  }
  if (data == "off2") {
    digitalWrite(D2, LOW);
    removeData(path);
  }
  if (data == "onall") {
    digitalWrite(D1, HIGH);
    digitalWrite(D2, HIGH);
    removeData(path);
  }
  if (data == "offall") {
    digitalWrite(D1, LOW);
    digitalWrite(D2, LOW);
    removeData(path);
  }
  
  if (data == "test") {
    String msg = "";
    msg += "123456";
    msg += ",";
    msg += "987654";
    String postData = "data=" + msg;

    HTTPClient http;
    http.begin("http://49.247.130.104/raw");
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(postData);
    http.POST(postData);
    http.end();
    removeData(path);
  }
}

void removeData(String path) {
  Firebase.remove("/queue" + path);
}
