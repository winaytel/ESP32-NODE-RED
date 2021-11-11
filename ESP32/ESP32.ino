#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#define DHTPIN 14 //pin al que se conectara
#define DHTTYPE DHT11 //se coloca el tipo

// INSTANCIA DLE SENSOR
DHT dht(DHTPIN, DHTTYPE); // enlace a la libreria


//**************************************
//*********** MQTT CONFIG **************
//**************************************
const char *mqtt_server = "voip.winaytel.com"; 
const int mqtt_port = 1883; //....
const char *mqtt_user = "master:mqttuser";
const char *mqtt_pass = "67266645-895f-438a-b247-48f184385eec"; 
const char *root_topic_subscribe = "esp32/input";
const char *root_topic_publish = "master/mqtt06/writeattributevalue/writeAttribute/6iUzmPqTk8aQi8kMpsqE7S"; 

//**************************************
//*********** WIFICONFIG ***************
//**************************************
const char* ssid = "ZTE";
const char* password =  "Peru1234";

//**************************************
//*********** GLOBALES   ***************
//**************************************

WiFiClient espClient;
PubSubClient client(espClient);
char msg[25];
long count=0;
int Led = 26;
String strTmp;
char chrTmp [5];
strTmp += t; //concatenando
String hostname = "ESP32"; //Nombre al hostname 

//************************
//** F U N C I O N E S ***
//************************
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
void setup_wifi();


void setup() {
  Serial.begin(115200);
  pinMode(Led, OUTPUT);
  
  dht.begin(); // se inicia el sensor
 
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
 float t = dht.readTemperature(); //leed datos de t° y humedad
 float h = dht.readHumidity(); //Lee la humedad

  Serial.print("Humedad: ");
  Serial.println(h);
  Serial.println(" %\t");
  Serial.print("Temperatura: ");
  Serial.println(t); 
   delay(30000);
   
   if (!client.connected()) {
     reconnect();
   }   
   if (client.connected()){
     strTmp.toCharArray(chrTmp,5);
     client.publish(root_topic_publish,chrTmp);
     count++;
     Serial.println("se envio los datos al broker ");    
   }
  client.loop();
}

//*****************************
//***    CONEXION WIFI      ***
//*****************************
void setup_wifi(){
  delay(10);
  // Nos conectamos a nuestra red Wifi
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str()); //define hostname
  Serial.println();
  Serial.print("Conectando a ssid: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Conectado a red WiFi!");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

//*****************************
//***    CONEXION MQTT      ***
//*****************************

void reconnect() {

  while (!client.connected()) {
    Serial.print("Intentando conexión Mqtt...");
    // Creamos un cliente ID
    String clientId = "mqtt06";
    // Intentamos conectar
      if (client.connect(clientId.c_str(),mqtt_user,mqtt_pass)) {      
      Serial.println("Conectado!");
      // Nos suscribimos
      } 
      else {
      Serial.print("falló :( con error -> ");
      Serial.print(client.state());
      Serial.println(" Intentamos de nuevo en 5 segundos");
      delay(5000);
      }
  }
}

//*****************************
//***       CALLBACK        ***
//*****************************

void callback(char* topic, byte* payload, unsigned int length){
  String incoming = "";
  Serial.print("Mensaje recibido desde -> ");
  Serial.print(topic);
  Serial.println("");
    for (int i = 0; i < length; i++) {
    incoming += (char)payload[i];
    
    }
     if (incoming == "1") {
      Serial.println("Encender Foco");
      digitalWrite(Led, HIGH);
     } 
    else if(incoming == "0") {
     Serial.println("Apagar Foco");
     digitalWrite(Led, LOW);
    } 
 incoming.trim();
 Serial.println("Mensaje -> " + incoming);
 
}
