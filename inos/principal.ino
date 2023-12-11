#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include "DHT.h"
#include "ESP32_Servo.h"

// Set consts
// WiFi Connection set

#define SSID "SEU SSDI" //nome da rede
#define PASSWORD "SUA SENHA" // senha da sua rede

// MQTT Connection set 

#define MQTT_ID "SEU ID" //Define o ID do ESP 
#define MQTT_BROKER "broker.hivemq.com" // broker grátis
#define MQTT_PORT 1883
#define MQTT_MILLIS_TOPIC "ifrj_cnit_estufatcc_millis" //topico millis 
#define MQTT_MOTOR_TOPIC "ifrj_cnit_motor"
#define MQTT_BOMBA_TOPIC "ifrj_cnit_bomba"
#define MQTT_UMISOLO_TOPIC "ifrj_cnit_umisolo"
#define MQTT_LIGHT_TOPIC "ifrj_cnit_light"
#define MQTT_TEMP_TOPIC "ifrj_cnit_temperatura" //topico da temperatura
#define MQTT_UMI_TOPIC "ifrj_cnit_umiar" //topico da umidade do ar


// Set ports
#define DHTPIN 12 //sensor DHT
#define PIN_MOTOR1 27 //define porta do servo
#define pinUS 34 // pino sensor de umidade do solo
#define pinLDR 35 // pino sensor LDR (luminosidade)
const int RelePin = 23; // pino do rele

Servo motor1; // cria o servo motor

// Set the type of DHT sensor
#define DHTTYPE DHT11 

// Define DHT
DHT dht(DHTPIN, DHTTYPE);

// Define net client
WiFiClient espClient; 

// Define mqtt client
PubSubClient MQTT(espClient);

// Vars
char millis_str[10] = "";

void setupWIFI(){
  if(WiFi.status() == WL_CONNECTED){
    return;
  } else {
    // Connect with WiFi

    Serial.println();
    Serial.print("Connecting to");
    Serial.println(SSID);

    WiFi.begin(SSID, PASSWORD);

    // Loop para checar a conexão

    while (WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    // Display connection info

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP Address:");
    Serial.println(WiFi.localIP());
    }
}

void setupMQTT(){
    // Config MQTT Broker connection

    MQTT.setServer(MQTT_BROKER, MQTT_PORT);
    MQTT.setCallback(mqtt_callback); // seta callback

    // Conn exec

    while (!MQTT.connected()){
        Serial.print("- MQTT Setup: Tentando se conectar ao Broker MQTT: ");
        Serial.println(MQTT_BROKER);

        if(MQTT.connect(MQTT_ID)){
            Serial.println("- MQTT Setup: Conectado com sucesso");
             MQTT.subscribe(MQTT_MOTOR_TOPIC); // lê o topico
             MQTT.subscribe(MQTT_BOMBA_TOPIC);
        } else {
            Serial.println("- MQTT Setup: Falha ao se conectar, tentando novamente em 2s");
            delay(2000);
        }
    }
}

void setup(){
    // Set baudrate of serial com
    Serial.begin(115200);

    // Pinmode
    pinMode(RelePin, OUTPUT);
    pinMode(pinUS, INPUT);
    pinMode(pinLDR, INPUT);


    motor1.attach(PIN_MOTOR1, 500, 2400);
    digitalWrite(RelePin, HIGH); 

    // Call setup wifi
    setupWIFI();
    // Call setup mqtt
    setupMQTT();
    // Start dht
    dht.begin();
}

void loop(){
    sprintf(millis_str, "%d", millis()); // store printf of data (millis)

    MQTT.publish(MQTT_MILLIS_TOPIC, millis_str); // publish in topic

     // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    int umidadeS = analogRead(pinUS);
    int luminosidade = analogRead(pinLDR);

    int porcentoL = map(luminosidade, 0, 4095, 0, 100);
    int porcentoUs = map(umidadeS, 4095, 0, 0, 100);

    MQTT.publish(MQTT_UMISOLO_TOPIC, String(porcentoUs).c_str()); // publica umidade do solo
    MQTT.publish(MQTT_LIGHT_TOPIC, String(porcentoL).c_str()); // publica luminosidade

    //Publica a TEMPERATURA
    if (isnan(t)) { //verifica se o sensor está mandando numero
     Serial.println(F("Failed to read from DHT sensor!"));
     return;
    } else {
      MQTT.publish(MQTT_TEMP_TOPIC, String(t, 1).c_str());
    }
    //Publica a HUMIDADE
    if (isnan(h)) { //verifica se o sensor está mandando numero
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    } else {
      MQTT.publish(MQTT_UMI_TOPIC, String(h, 0).c_str());
    }

    if (porcentoUs <= 10) {
      digitalWrite(RelePin, LOW);  //liga o pino
      Serial.println("LIGANDO BOMBA");
      delay(1000);
      digitalWrite(RelePin, HIGH); //desliga
    } else {
      digitalWrite(RelePin, HIGH); //aciona o pino
      Serial.println("DESLIGANDO BOMBA");
    }

    setupWIFI();
    setupMQTT();
    MQTT.loop();
    delay(2000);
}

void mqtt_callback(char* topic, byte* payload, unsigned int length)
{
  String msg;
  Serial.print("Topic: ");
  Serial.println(topic);

  //obtem a string do payload recebido
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }

  Serial.println(msg);
  //Controlando teto motor 
  if (msg.equals("M1")) {
     for (int pos = 180; pos >= 0; pos -= 1) {
      motor1.write(pos); //motor roda 160 graus
      delay(5);
      }
      Serial.println("MOTOR INDO");
    } else if (msg.equals("M0")) {
      for (int pos = 0; pos <= 180; pos += 1) {
      motor1.write(pos);
      delay(5);
      } 
      Serial.println("MOTOR VOLTANDO");
    }
  if (msg.equals("B1")) {
      digitalWrite(RelePin, LOW);  //desativa o pino
      Serial.println("LIGANDO BOMBA");
    } else if (msg.equals("B0")) {
      digitalWrite(RelePin, HIGH); //aciona o pino
      Serial.println("DESLIGANDO BOMBA");
    }
}
