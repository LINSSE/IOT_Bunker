/*
 Sensor de temperatura humedad y luz

 Reporta por MQTT cada 2 segundos los valores obtenidos por los sensores.

 Sensores utilizados:
    DHT11 (D4)
    LDR   (A0)
 

*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
#include <ArduinoJson.h>


DHTesp dht;

// Parametros de red y servidor de facena - 9 de julio
const char* ssid = "FACENA-INFO";
const char* password = "F4CENAUNNE";
const char* mqtt_server = "10.40.60.12";

// Parametros de red y servidor de facena - rectorado
//const char* ssid = "UNNE-Invitado";
//const char* password = "UnneInv2017";
//const char* mqtt_server = "10.40.60.12";

// Parametros de red y servidor para pruebas
//const char* ssid = "Fibertel WiFi027 2.4GHz";
//const char* password = "0043591365";
//const char* mqtt_server = "sambrana.com.ar";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


void setup() {
  //pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(A0, OUTPUT);
  dht.setup(D4, DHTesp::DHT11);
  
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
}

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
}

void callback(char* topic, byte* payload, unsigned int length) {

  String dato="";
  char c;

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
    dato += c;
  }
  Serial.println();
  Serial.print("------json------");Serial.println(dato);
  dato = "";


}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Sensor_bunker")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("facena", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
    ++value;
    
    float Humedad = dht.getHumidity();
    float Temperatura = dht.getTemperature();
    char hum[8]; // Buffer big enough for 7-character float
    dtostrf(Humedad, 6, 2, hum); 
    char tem[8]; // Buffer big enough for 7-character float
    dtostrf(Temperatura, 6, 2, tem); 
    int luz = analogRead(A0);

    StaticJsonBuffer<200> doc;
    JsonObject& root = doc.createObject();
    //root["Nodo"] = "Bunker";
    root["Nodo"] = String(ESP.getChipId());
    root["temp"] = tem;
    root["hum"] = hum;
    root["luz"] = luz;
    String output;
    root.printTo(output);
    
    Serial.print("Publish message: ");
    Serial.println(output);
    //snprintf (msg, 100, output);
    char msg[200];
    output.toCharArray(msg, 200);
    client.publish("facena", msg);
    //client.publish("facena", "output");
  }
}
