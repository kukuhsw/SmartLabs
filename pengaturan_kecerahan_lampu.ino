#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//Konfigurasi WiFi
#define WLAN_SSID       "HELLU"   //Nama 
#define WLAN_PASS       "******"  //Password
  
#define AIO_SERVER      "192.168.43.197"  // Alamat Broker
#define AIO_SERVERPORT  1883              // sebaiknya gunakan 8883 for SSL
#define AIO_USERNAME    ""                // nama user
#define AIO_KEY         ""                // password
#define LEDPIN D6

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Subscribe brightness = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/brightness");
 
 
void MQTT_connect();
 
void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(LEDPIN,OUTPUT);
  Serial.println(F("Pengaturan Kecerahan LED"));
  Serial.println(); Serial.println();
  Serial.print("Terkoneksi dengan");
  Serial.println(WLAN_SSID);
 
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Terkoneksi");
  Serial.println("Alamat IP: ");
  Serial.println(WiFi.localIP());
  mqtt.subscribe(&brightness);
}
 
uint32_t x=0;
 
void loop() {
  MQTT_connect();
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &brightness) {
      Serial.print(F("Got: "));
      String brightnessValue = (char *)brightness.lastread;
      Serial.println(brightnessValue);
      analogWrite(LEDPIN, brightnessValue.toInt());
       
    }
  }
 
}
 
void MQTT_connect() {
  int8_t ret;
  if (mqtt.connected()) {
    return;
  }
 
  Serial.print("Mencari koneksi MQTT");
 
  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { 
       Serial.println(mqtt.connectErrorString(ret));
       mqtt.disconnect();
       delay(5000);  
       retries--;
       if (retries == 0) {
         while (1);
       }
  }
  Serial.println("Terkoneksi dengan MQTT Broker");
}
