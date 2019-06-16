#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "ESP8266WiFi.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define PIR_PIN_SIG 5
#define VIB_PIN_SIG 16
#define BUZZER_PIN_SIG 12
#define BUTTON_PIN_SIG 4

#define HOSTIFTTT "maker.ifttt.com"
#define EVENTO "sms"
#define IFTTTKEY "y27saSU_6mz1Ja8T27fCH"

#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
String dest;
ESP8266WiFiMulti WiFiMulti;
HTTPClient http;




WiFiClientSecure client;

char myssid[] = "g";         // your network SSID (name)
char mypass[] = "123456789"; 

//Credentials for Google GeoLocation API...
const char* Host = "www.googleapis.com";
String thisPage = "/geolocation/v1/geolocate?key=";
String key = "AIzaSyBb4rCNjdMLCnZCZU8De9GX1_sGqILLT20";



int status = WL_IDLE_STATUS;
String jsonString = "{\n";

double latitude    = 0.0;
double longitude   = 0.0;
double accuracy    = 0.0;
int more_text = 1;

int pir_op = LOW;
int vib_op = LOW;
int isAnybodyIn = 0;

boolean isInCar = false;

void setup() {
  Serial.begin(115200);
  WiFiMulti.addAP("g","123456789");
  delay(5000);

  // put your setup code here, to run once:
  pinMode(D6, INPUT_PULLUP);
  //attachInterrupt(D6, resetAll, FALLING);
  Serial.begin(115200);
  pinMode(PIR_PIN_SIG, INPUT);
  pinMode(VIB_PIN_SIG, INPUT);
  pinMode(BUZZER_PIN_SIG, OUTPUT);

  /********** SETTING CONNECTION WITH WIFI TO GET LATITUDE AND LONGITUDE **********/
  Serial.println("Start");
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(myssid);
  WiFi.begin(myssid, mypass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
  Serial.print("Connected to ");
  Serial.println(myssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
  
}

void loop() {
  // put your main code here, to run repeatedly:
    delay(2000);
    if (accidentHappened()) {
      Serial.println("Accident Happened");
      for (int i = 0; i < 10; i++) {
         delay(2000); 
          if(isInsideTheCar()){
            Serial.println("Some is inside car");
            
            while (latitude == 0 && longitude == 0)
            {
              setLatLng();
            }
            delay(5000);
            sendMessage();
            while(true){
            }
        }  
      }
   }
  //}
}


boolean isInsideTheCar() {
    
    //boolean motion_detected = false;
    //boolean lockTime = false; 
    if (digitalRead(PIR_PIN_SIG)) 
      return true;
    return false;
}

void sendMessage() {
  ESP8266WiFiMulti WiFiMulti;
  HTTPClient http;
  String message; //Maps API.
  message = "https://www.google.com/maps/search/?api=1&query=" ;
  message = message +String(latitude);
  message = message + ",";
  message = message +String(longitude);
  String dest;

  Serial.begin(115200);
  WiFiMulti.addAP("g","123456789");
  delay(5000);
  if(WiFiMulti.run() == WL_CONNECTED){
  
  dest="https://maker.ifttt.com/trigger/sms/with/key/y27saSU_6mz1Ja8T27fCH/?value1=+919892642557&value2="+message;

  Serial.println(dest);
  http.begin(dest,"AA:75:CB:41:2E:D5:F9:97:FF:5D:A0:8B:7D:AC:12:21:08:4B:00:8C");
  
  int httpCode = http.GET();
  if(httpCode>0){
    Serial.printf("[HTTP] GET... code: %d\n",httpCode);
    if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();
    }
    else{
      Serial.printf("[HTTP GET... failed, error: %s\n",http.errorToString(httpCode).c_str());
    }
    http.end();
  }  
}

  Serial.println("Accident Happened "+message);
}

// SETS VALUE OF LATITUDE AND LONGITUDE.
void setLatLng() {
  char bssid[6];
  DynamicJsonBuffer jsonBuffer;
  Serial.println("scan start");
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found...");

    if (more_text) {
      // Print out the formatted json...
      Serial.println("{");
      Serial.println("\"homeMobileCountryCode\": 234,");  // this is a real UK MCC
      Serial.println("\"homeMobileNetworkCode\": 27,");   // and a real UK MNC
      Serial.println("\"radioType\": \"gsm\",");          // for gsm
      Serial.println("\"carrier\": \"Vodafone\",");       // associated with Vodafone
      //Serial.println("\"cellTowers\": [");                // I'm not reporting any cell towers
      //Serial.println("],");
      Serial.println("\"wifiAccessPoints\": [");
      for (int i = 0; i < n; ++i)
      {
        Serial.println("{");
        Serial.print("\"macAddress\" : \"");
        Serial.print(WiFi.BSSIDstr(i));
        Serial.println("\",");
        Serial.print("\"signalStrength\": ");
        Serial.println(WiFi.RSSI(i));
        if (i < n - 1)
        {
          Serial.println("},");
        }
        else
        {
          Serial.println("}");
        }
      }
      Serial.println("]");
      Serial.println("}");
    }
    Serial.println(" ");
  }
  // now build the jsonString...
  jsonString = "{\n";
  jsonString += "\"homeMobileCountryCode\": 234,\n"; // this is a real UK MCC
  jsonString += "\"homeMobileNetworkCode\": 27,\n";  // and a real UK MNC
  jsonString += "\"radioType\": \"gsm\",\n";         // for gsm
  jsonString += "\"carrier\": \"Vodafone\",\n";      // associated with Vodafone
  jsonString += "\"wifiAccessPoints\": [\n";
  for (int j = 0; j < n; ++j)
  {
    jsonString += "{\n";
    jsonString += "\"macAddress\" : \"";
    jsonString += (WiFi.BSSIDstr(j));
    jsonString += "\",\n";
    jsonString += "\"signalStrength\": ";
    jsonString += WiFi.RSSI(j);
    jsonString += "\n";
    if (j < n - 1)
    {
      jsonString += "},\n";
    }
    else
    {
      jsonString += "}\n";
    }
  }
  jsonString += ("]\n");
  jsonString += ("}\n");
  //--------------------------------------------------------------------

  Serial.println("");

  WiFiClientSecure client;

  //Connect to the client and make the api call
  Serial.print("Requesting URL: ");
  Serial.println("https://" + (String)Host + thisPage + "AIzaSyCYNXIYINPmTNIdusMjJloS4_BXSOff1_g");
  Serial.println(" ");
  if (client.connect(Host, 443)) {
   Serial.println("Connected");
    client.println("POST " + thisPage + key + " HTTP/1.1");
    client.println("Host: " + (String)Host);
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("User-Agent: Arduino/1.0");
    client.print("Content-Length: ");
    client.println(jsonString.length());
    client.println();
    client.print(jsonString);
    delay(500);
  }

  //Read and parse all the lines of the reply from server
  while (client.available()) {
    String line = client.readStringUntil('\r');
    if (more_text) {
      Serial.print(line);
    }
    JsonObject& root = jsonBuffer.parseObject(line);
    if (root.success()) {
      latitude    = root["location"]["lat"];
      longitude   = root["location"]["lng"];
      accuracy   = root["accuracy"];
    }
  }

  Serial.println("closing connection");
  Serial.println();

  Serial.print("Latitude = ");
  Serial.println(latitude, 6);
  Serial.print("Longitude = ");
  Serial.println(longitude, 6);
  Serial.print("Accuracy = ");
  Serial.println(accuracy);
}

boolean accidentHappened() {
  long duration = 0;

  if (digitalRead(VIB_PIN_SIG)) 
    return true;
    
  /*Serial.println((pulseIn(VIB_PIN_SIG, HIGH)/1000));
  duration = (pulseIn(VIB_PIN_SIG, HIGH)/1000);
  if (duration > 3) {
    if ((pulseIn(PIR_PIN_SIG, HIGH)/1000) > 3) {
      Serial.println("Start Send Message.");
    }
  }*/
  
  return false;
}

void resetAll() {
  ESP.restart();
}
