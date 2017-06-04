/*
*----YOUTUBE: Mikro Maker----- 
*DHT11 Temperature&Humidity Sensor 
*MQTT Example
*https://github.com/mikromaker/esp8266
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include "DHT.h"

#define DHTPIN 4     // what digital pin the DHT22 is conected to
#define DHTTYPE DHT11   // there are multiple kinds of DHT sensors

DHT dht(DHTPIN, DHTTYPE);
const char* mqtt_server = "test.mosquitto.org";
const char* ssid = "**********";
const char* password = "******";
int timeSinceLastRead = 0;
unsigned char versiontimer = 200;
char mqtt_address[256];

WiFiClient espClient;

PubSubClient client(espClient);
char msg[128];
byte mac[6];

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  WiFi.macAddress(mac);
  sprintf(mqtt_address,"test/%2X%2X%2X%2X%2X%2X",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]); 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("MikroMaker")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void read_temp() {
     
     char message_buff[512];
    // Report every 2 seconds.
    if(timeSinceLastRead > 2000) {
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
      // Read temperature as Fahrenheit (isFahrenheit = true)
      float f = dht.readTemperature(true);
  
      // Check if any reads failed and exit early (to try again).
      if (isnan(h) || isnan(t) || isnan(f)) {
        Serial.println("Failed to read from DHT sensor!");
        timeSinceLastRead = 0;
        return;
      }
  
      sprintf(message_buff,"{ \"temperature\":%d.%02d, \"humidity\":%d.%02d }",
      (int)t,(int)(t*100)%100,(int)h,(int)(h*100)%100);
      if(client.publish(mqtt_address,message_buff))
        Serial.println("Success");
      else
        Serial.println("Fail");
    
      Serial.println(message_buff);
      timeSinceLastRead = 0;
    }
    delay(100);
    timeSinceLastRead += 100;

}

void setup() {
//  timeClient.begin();
  Serial.begin(9600);
  Serial.setTimeout(2000);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  // Wait for serial to initialize.
  while(!Serial) { }

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");

}


void loop() {
   if (!client.connected()) {
    reconnect();
  }
  read_temp();
  client.loop();
}
