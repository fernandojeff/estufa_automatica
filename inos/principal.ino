#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPmDNS.h>
#include <DHT.h>

// Set consts
// WiFi Connection set

#define SSID "Deus e fiel"
#define PASSWORD "210305Ma#"

// MQTT Connection set 

#define MQTT_ID "estufatcc23-ifrjesp32group-ifrjcnit"
#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_MILLIS_TOPIC "ifrj_cnit_estufatcc_millis"
#define MQTT_LED_TOPIC "ifrj_cnit_estufatcc_led"
#define MQTT_TEMP_TOPIC "ifrj_cnit_estufatcc_temperatura" //topico da temperatura
#define MQTT_HUMI_TOPIC "ifrj_cnit_estufatcc_humidade" //topico da humidade do ar
#define MQTT_LUMINOSIDADE_TOPIC "ifrj_cnit_estufatcc_luminosidade" //topico da luminosidade
#define MQTT_HUMISOLO_TOPIC "ifrj_cnit_estufatcc_humidadesolo" //topico da humidade do solo

// Set ports
#define LED_R 2 //led interno
#define DHTPIN 12 //sensor DHT
#define LDRPIN 4 //sensor LDR
#define pinUS 14 //sensor de humidade do solo

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
    MQTT.setCallback(mqtt_ifrj_callback); 

    // Conn exec

    while (!MQTT.connected()){
        Serial.print("- MQTT Setup: Tentando se conectar ao Broker MQTT: ");
        Serial.println(MQTT_BROKER);

        if(MQTT.connect(MQTT_ID)){
            Serial.println("- MQTT Setup: Conectado com sucesso");
             MQTT.subscribe(MQTT_LED_TOPIC);
        } else {
            Serial.println("- MQTT Setup: Falha ao se conectar, tentando novamente em 2s");
            delay(2000);
        }
    }
}

void setup(void){
    // Set baudrate of serial com
    Serial.begin(115200);

    // Pinmode
    pinMode(LED_R, OUTPUT);
    pinMode(LDRPIN, INPUT);
    pinMode(pinUS, INPUT);

    // Call setup wifi
    setupWIFI();
    // Call setup mqtt
    setupMQTT();
    // Start dht
    dht.begin();
}

void loop(void){
    sprintf(millis_str, "%d", millis()); // store printf of data (millis)

    MQTT.publish(MQTT_MILLIS_TOPIC, millis_str); // publish in topic
  
    setupWIFI();
    setupMQTT();
    MQTT.loop();
    //delay(2000);

    //VARIAVEIS
    int luminosidade = analogRead(LDRPIN);
    int humiSolo = analogRead(pinUS);

     // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    
    // transforma em porcentagem
    int porcentoL = map(luminosidade, 0, 4095, 0, 100);
    int porcentoUs = map(humiSolo, 4095, 0, 0, 100);

    MQTT.publish(MQTT_LUMINOSIDADE_TOPIC, String(porcentoL).c_str()); //publica a luminosidade
    MQTT.publish(MQTT_HUMISOLO_TOPIC, String(porcentoUs).c_str()); //publica humidade do solo

    //Publica a TEMPERATURA
    if (isnan(t)) { //verifica se o sensor está mandando numero
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    } else {
      MQTT.publish(MQTT_TEMP_TOPIC, String(t).c_str());
    }
    //Publica a HUMIDADE
    if (isnan(h)) { //verifica se o sensor está mandando numero
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    } else {
      MQTT.publish(MQTT_HUMI_TOPIC, String(h).c_str());
    }
}

void mqtt_ifrj_callback(char* topic, byte* payload, unsigned int length)
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
  //Controlando LED RED
  if (msg.equals("1"))
    {
      digitalWrite(LED_R, HIGH);
      Serial.println("L recebido");
    } else if (msg.equals("0"))
    {
      digitalWrite(LED_R, LOW);
      Serial.println("D recebido");
    }
}