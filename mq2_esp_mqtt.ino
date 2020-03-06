#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>
#include <Arduino.h>

// Konfigurasi WiFi
#define ssid "HELLU"//Nama
#define password "*******"//Password

// Konfigurasi MQTT Broker
#define mqtt_server "192.168.43.234" // Alamat MQTT Broker
#define mqtt_topic_pub "gas"   
#define mqtt_topic_sub "gas"
#define mqtt_user ""   
#define mqtt_pwd ""
//
const uint16_t mqtt_port =  1883; 

int redLed = 16;
int greenLed = 5;
int buzzer = 4;
int smoke = A0;

int calibrationTime = 30;

Servo myservo;
int sensorThres = 200;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50]; 
int value = 0;

void setup() {
  Serial.begin(115200);
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(smoke, INPUT);
  myservo.attach(14);
  myservo.write(0);

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
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived []");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Menerima koneksi MQTT...");
    if (client.connect("ESP8266Client",mqtt_user, mqtt_pwd)) {
      Serial.println("Terkoneksi");
      client.publish(mqtt_topic_pub, "ESP_reconnected");
      client.subscribe(mqtt_topic_sub);
    } else {
      Serial.print("Gagal Terhubung, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  int analogSensor = analogRead(smoke);
  if (isnan(analogSensor)) {
   Serial.println("Gagal menerima data dari sensor");
   return;
  }

    long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    static char labKimia[7];
    dtostrf(analogSensor, 6, 2, labKimia);
    client.publish("sensor/gas", labKimia);
 
    Serial.print(" *C\t gas: ");   Serial.print(analogSensor);
    Serial.print("\n ");
  }
  
  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  // Check dulu nilai thresholdnya 
  if (analogSensor > sensorThres)
  {
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, LOW);
      for (int i=0; i>60; i++);
      {
          myservo.write(180);//gerak servo 180 derajat
          delay(1000);//tunggu 1s
      }
    tone(buzzer, 1000, 200);
  }
  else
  {
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, HIGH);
      for (int i=0; i>60; i++);
    {
        myservo.write(0);//reset posisi
        delay(1000);//tunggu 1s
    }
    noTone(buzzer);
  }
  delay(300);
}
