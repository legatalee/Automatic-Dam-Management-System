#include <ESP8266WiFi.h>
#include <WiFiUDP.h>

#include <FirebaseArduino.h>
//#include <SoftwareSerial.h>

#define FIREBASE_HOST "automatic-dam.firebaseio.com"
#define FIREBASE_AUTH "3rxeL5tbMRMQn7qecowaJumq9XDd7MRH1DS6Hd3l"

//#define WIFI_SSID "legatalee_laptop"
//#define WIFI_PASSWORD "dnflwlq4!"
//#define WIFI_SSID "DimiFi_2G"
//#define WIFI_PASSWORD "newdimigo"
#define WIFI_SSID "dimi_flow_ai"
#define WIFI_PASSWORD "dimidimi"

WiFiUDP Udp;

void setup() {
  pinMode(D7, OUTPUT);
  pinMode(D6, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D1, INPUT);

  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Udp.begin(12345);

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  delay(50);
  Firebase.stream("/queue");
}

String path;
String data;
int cnt = 0;

unsigned long prev_time = 0;
float duration, level;
String echo = "";

void loop() {
  String path = "";
  String data = "";

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
    digitalWrite(D7, HIGH);
    removeData(path);
  }
  if (data == "off1") {
    digitalWrite(D7, LOW);
    removeData(path);
  }
  if (data == "on2") {
    digitalWrite(D6, HIGH);
    removeData(path);
  }
  if (data == "off2") {
    digitalWrite(D6, LOW);
    removeData(path);
  }
  if (data == "onall") {
    digitalWrite(D7, HIGH);
    digitalWrite(D6, HIGH);
    removeData(path);
  }
  if (data == "offall") {
    digitalWrite(D7, LOW);
    digitalWrite(D6, LOW);
    removeData(path);
  }

  digitalWrite(D2, HIGH);
  unsigned long current_time = millis();
  if (current_time - prev_time > 15) {
    digitalWrite(D2, LOW);
    duration = pulseIn(D1, HIGH);
    level = map(duration, 970, 50, 0, 180);
    //    level = duration;
    if (level < 0)level = 0; echo = String(level / 10);
    echo = echo.substring(0, echo.length() - 1);

    String postData = "data=" + echo + ',' + String(analogRead(A0) * (5.0 / 1024.0));
    Serial.println(postData);

    Udp.beginPacket("192.168.137.1", 3000);
    Udp.write(postData.c_str());
    Udp.endPacket();

    prev_time = millis();
  }
}

void removeData(String path) {
  Firebase.remove("/queue" + path);
}
