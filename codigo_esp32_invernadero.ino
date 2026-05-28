#include <WiFi.h>          // Conexión WiFi
#include <PubSubClient.h>  // Comunicación MQTT
#include <Wire.h>          // Comunicación I2C
#include <BH1750.h>        // Sensor de luminosidad BH1750
#include "DHT.h"           // Sensor DHT22


//  CONFIGURACIÓN WIFI Y MQTT
const char* ssid        = "TFG-INV";
const char* password    = "******";
const char* mqtt_server = "192.168.1.139";
const int   mqtt_port   = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

//  CONFIGURACIÓN DE PINES
// Sensor DHT22
#define PIN_DHT    27
#define DHTTYPE    DHT22
// Sensor humedad suelo
#define PIN_SUELO  34
// LEDs indicadores WiFi
#define LED_ROJO   18
#define LED_VERDE  19
// Relés
#define RELE_VENTILADOR 16
#define RELE_BOMBA      17
// Relés activos en LOW
#define RELE_ON   LOW
#define RELE_OFF  HIGH

//  OBJETOS DE SENSORES   
DHT dht(PIN_DHT, DHTTYPE);
BH1750 lightMeter;

//VARIABLES DE CONTROL
unsigned long lastMsg = 0;
const long intervaloEnvio = 5000;

//ACTUALIZACIÓN LEDS WIFI
void actualizarLedsWifi() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_ROJO, LOW);
  } else {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_ROJO, HIGH);
  }
}

//CONEXIÓN WIFI
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  digitalWrite(LED_ROJO, HIGH);
  digitalWrite(LED_VERDE, LOW);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  actualizarLedsWifi();
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.print("IP ESP32: ");
  Serial.println(WiFi.localIP());
}

// RECEPCIÓN MQTT 
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  msg.trim();
  msg.toLowerCase();
  Serial.print("Mensaje MQTT [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(msg);
  if (String(topic) == "invernadero/ventilador") {  //  CONTROL VENTILADOR
    if (msg == "on") {
      digitalWrite(RELE_VENTILADOR, RELE_ON);
    }
    if (msg == "off") {
      digitalWrite(RELE_VENTILADOR, RELE_OFF);
    }
  }
  if (String(topic) == "invernadero/bomba") { //  CONTROL BOMBA
    if (msg == "on") {
      digitalWrite(RELE_BOMBA, RELE_ON);
    }
    if (msg == "off") {
      digitalWrite(RELE_BOMBA, RELE_OFF);
    }
  }
}

//  RECONEXIÓN MQTT
void reconnect() {
  while (!client.connected() && WiFi.status() == WL_CONNECTED) {
    Serial.print("Conectando a MQTT...");
    if (client.connect("ESP32-invernadero")) {
      Serial.println(" conectado");
      client.subscribe("invernadero/ventilador");
      client.subscribe("invernadero/bomba");
    } else {
      Serial.print(" fallo, rc=");
      Serial.print(client.state());
      Serial.println(" reintentando en 5 segundos");
      delay(5000);
    }
  }
}

//  CONFIGURACIÓN INICIAL 
void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(RELE_VENTILADOR, OUTPUT); //  CONFIGURACIÓN RELÉS 
  pinMode(RELE_BOMBA, OUTPUT);
  digitalWrite(RELE_VENTILADOR, RELE_OFF);
  digitalWrite(RELE_BOMBA, RELE_OFF);
  pinMode(LED_ROJO, OUTPUT);  //  CONFIGURACIÓN LEDS
  pinMode(LED_VERDE, OUTPUT);
  digitalWrite(LED_ROJO, LOW);
  digitalWrite(LED_VERDE, LOW);
  Wire.begin(21, 22); //  INICIO I2C 
  dht.begin();  //  INICIO SENSORES
  lightMeter.begin();
  setup_wifi(); //  CONEXIÓN WIFI
  client.setServer(mqtt_server, mqtt_port); //  CONFIGURACIÓN MQTT
  client.setCallback(callback);
  Serial.println("Sistema iniciado correctamente");
}


//  ENVÍO DATOS SENSORES
void enviarDatosSensores() {
  float temperatura = dht.readTemperature();  // Lectura temperatura
  float humedad = dht.readHumidity(); // Lectura humedad ambiente
  float luz = lightMeter.readLightLevel();  // Lectura luminosidad
  int suelo = analogRead(PIN_SUELO);  // Lectura humedad suelo
  Serial.print("Temperatura: ");  //  MOSTRAR DATOS POR SERIAL
  Serial.print(temperatura);
  Serial.print(" °C | Humedad: ");
  Serial.print(humedad);
  Serial.print(" % | Luz: ");
  Serial.print(luz);
  Serial.print(" lux | Suelo: ");
  Serial.println(suelo);
  String payload = "{"; //  CREACIÓN JSON 
  payload += "\"temperatura\":";
  payload += String(temperatura, 2);
  payload += ",";
  payload += "\"humedad\":";
  payload += String(humedad, 2);
  payload += ",";
  payload += "\"luz\":";
  payload += String(luz, 2);
  payload += ",";
  payload += "\"suelo\":";
  payload += String(suelo);
  payload += "}";
  if (WiFi.status() == WL_CONNECTED && client.connected()) { // ENVÍO MQTT 
    client.publish("invernadero/sensores", payload.c_str());
    Serial.println("Datos enviados por MQTT");
  }
}


//  BUCLE PRINCIPAL
void loop() {
  actualizarLedsWifi();
  if (WiFi.status() == WL_CONNECTED) {  // COMPROBAR MQTT
    if (!client.connected()) {
      reconnect();
    }
    client.loop();
  }

//  ENVÍO PERIÓDICO
  unsigned long now = millis();
  if (now - lastMsg > intervaloEnvio) {
    lastMsg = now;
    enviarDatosSensores();
  }
}