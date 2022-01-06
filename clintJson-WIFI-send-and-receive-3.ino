#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char WIFI_SSID[] = "chuba";
const char WIFI_PSK[] = "chuba1234";

// Remote site information
const char http_site[] = "chutrain-001-site1.itempurl.com";
const int http_port = 80;
int x = 0;
String url;
String temp1 = "1", temp2 = "0", temp3 = "0", hum1 = "0", hum2 = "0", hum3 = "0", pas1 = "0", pas2 = "0", pas3 = "0", engtemp = "0";
String vibr = "0";

WiFiClient client;

unsigned long lastConnectionTime = 0;                // last time we connected to Xively
const unsigned long connectionInterval = 4000;      // delay between connecting to Cosm in milliseconds
//sending data to the cloud
void sendrequest() {
  url = "GET /Dashboard/post?";
  url += "t1=";
  url += temp1;
  url += "&h1=";
  url += hum1;
  url += "&p1=";
  url += pas1;
  url += "&t2=";
  url += temp2;
  url += "&h2=";
  url += hum2;
  url += "&p2=";
  url += pas2;
  url += "&t3=";
  url += temp3;
  url += "&h3=";
  url += hum3;
  url += "&p3=";
  url += pas3;
  url += "&et=";
  url += engtemp;
  url += "&v=";
  url += vibr;

  client.connect(http_site, http_port);
  client.print(url);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(http_site);
  client.println("Connection: close");
  client.println();

  //  Serial.print(http_site);
  //Serial.println(url);
}

void getRequest() {
  client.connect(http_site, http_port);
  client.print("GET /Dashboard/Get");
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(http_site);
  client.println("Connection: close");
  client.println();
}

void setup() {
  Serial.begin(9600);
  delay(10);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    //Serial.print(".");
  }
  //  Serial.println("");
  //  Serial.println("WiFi connected");
  //  Serial.println("IP address: ");
  //  Serial.println(WiFi.localIP());
  sendrequest();
}

void loop() {

  if (millis() - lastConnectionTime > connectionInterval) {
    if (x == 0) {
      getRequest();
      lastConnectionTime = millis();
    }
    //    else if (x == 1) {
    //      sendrequest();
    //      lastConnectionTime = millis();
    //      x = 0;
    //    }
  }

  if ( client.available() ) {
    //Serial.println(client.readString());
    String pos[3] = client.readStringUntil('#');
    String remoteRequest = pos[1];
    Serial.print(remoteRequest);
    Serial.println("#");
  }

  //37.45#78#2#38.2#85#1#36.25#71#4#48#
  if ( Serial.available()) {
    String pos[11] = Serial.readStringUntil('#');
    temp1 = pos[0]; hum1 = pos[1]; pas1 = pos[2];
    temp2 = pos[3]; hum2 = pos[4]; pas2 = pos[5];
    temp3 = pos[6]; hum3 = pos[7]; pas3 = pos[8];
    engtemp = pos[9];
    sendrequest();
    lastConnectionTime = millis();
    //      x=1;
  }
}
