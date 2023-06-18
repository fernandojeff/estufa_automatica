#include "DHT.h"
#include "ESP32_Servo.h"

#define pinLDR 4  

#define pinDHT 12
#define DHTTYPE DHT11
DHT dht(pinDHT, DHTTYPE);

#define pinUS 14

#define PIN_MOTOR1 27 //define porta do servo

Servo motor1;

const int RelePin = 23; // pino ao qual o Módulo Relé está conectado
int incomingByte;      // variavel para ler dados recebidos pela serial

void setup(){
  pinMode(pinLDR, INPUT);
  pinMode(pinUS, INPUT);
  Serial.begin(115200);
  dht.begin();

  pinMode(RelePin, OUTPUT); // seta o pino como saída
  digitalWrite(RelePin, HIGH); // seta o pino com nivel logico baixo

  motor1.attach(PIN_MOTOR1, 500, 2400); //
}

void loop(){
  int luminosidade = analogRead(pinLDR);
  float humidadeAr = dht.readHumidity();
  float temperatura = dht.readTemperature();
  int umidadeS = analogRead(pinUS);

  int porcentoL = map(luminosidade, 0, 4095, 0, 100);
  int porcentoUs = map(umidadeS, 4095, 0, 0, 100);

  incomingByte = Serial.read(); //lê o monitor serial

  if (incomingByte == 'A') { // LIGA O MICRO SERVO CASO APERTE "A"
     for (int pos = 180; pos >= 0; pos -= 1) {
    motor1.write(pos); //motor roda 160 graus
    delay(5);
    }
    Serial.println("MOTOR INDO");
  }  

  if (porcentoL == 'D') {  // LIGA O MICRO SERVO CASO APERTE "D"
    for (int pos = 0; pos <= 180; pos += 1) {
    motor1.write(pos);
    delay(5);
    } 
    Serial.println("MOTOR VOLTANDO");
  }

  if (porcentoUs <= 20) {     //SE HUMIDADE MENOR DO QUE 20 ATIVA
    digitalWrite(RelePin, LOW);  //desativa o pino
    Serial.println("LIGANDO BOMBA");
  } else {
    digitalWrite(RelePin, HIGH); //aciona o pino
    Serial.println("DESLIGANDO BOMBA");
  }
  
  Serial.println("Luminosidade: " + String(porcentoL) + "%");
  Serial.println("Umidade do Ar: " + String(umidadeAr, 0) + "%");
  Serial.println("Temperatura: " + String(temperatura, 1) + "°C");
  Serial.println("Umidade do Solo: " + String(porcentoUs) + "%");
  Serial.println("===============");
  delay(2000); 
}