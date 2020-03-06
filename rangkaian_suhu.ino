#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include<dht.h>
#define DHT11_PIN D4
dht DHT;


// Identifikasi Access Point
const char* ssid = "HELLU"; //Nama Hostpoot
const char* password = "L4L4L4"; //Password
 
ESP8266WebServer server(80);   //instantiate server at port 80 (http port)
 
String page = "";
int LEDPin = D3;
void setup(void){

  //Pengaturan I/O LED
  pinMode(LEDPin, OUTPUT);
  digitalWrite(LEDPin, LOW);
   
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password); //Mulai mengkoneksikan perangkat
  Serial.println("");
 
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

 
   
  server.on("/", [](){
    server.send(200, "text/html", page);
  });
  server.on("/LEDNyala", [](){
    page = "<h1>SmartLabs MQTT</h1><p><strong>LED Nyala</strong></p>";
  page +="<p><a href=\"LEDNyala\"><button>Turn on LED</button></a>&nbsp;<a href=\"LEDMati\"><button>LED Mati</button></a></p>";
  page +="<p><a href=\"AmbilSuhu\"><button>Tampilan Suhu</button>";
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, HIGH);
    delay(1000);
  });
  server.on("/LEDMati", [](){
      page = "<h1>SmartLabs MQTT</h1><p><strong>LED mati</strong></p>";
  page +="<p><a href=\"LEDNyala\"><button>Nyalakan LED</button></a>&nbsp;<a href=\"LEDMati\"><button>Matikan LED</button></a></p>";
  page +="<p><a href=\"AmbilSuhu\"><button>Tampilan Suhu</button>";
    server.send(200, "text/html", page);
    digitalWrite(LEDPin, LOW);
    delay(1000); 
  });
   server.on("/AmbilSuhu", [](){
      int chk = DHT.read11(DHT11_PIN);
     String msg="<p>Data Suhu : ";
     msg= msg+ DHT.temperature;
     msg = msg+" C ;Data Kelembaban : " ;
     msg=msg+DHT.humidity ;
     msg=msg+"%</p>";
 
   
  page = "<h1>SmartLabs MQTT</h1>"+msg;
  
  page +="<p><a href=\"LEDNyala\"><button>Nyalakan LED</button></a>&nbsp;<a href=\"LEDMati\"><button>Matikan LED</button></a></p>";
  
  page +="<p><a href=\"AmbilSuhu\"><button>Tampilan Suhu</button>";
    server.send(200, "text/html", page);
    delay(1000); 
  });
  server.begin();
  Serial.println("Web server sudah aktif!");
}
 
void loop(void){
  server.handleClient();
}
