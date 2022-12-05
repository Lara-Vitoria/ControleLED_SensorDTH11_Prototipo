#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "DHT.h"

// define tipo e pino do sensor
#define DHTTYPE DHT11
#define dht_dpin 2  //pino D4
DHT dht(dht_dpin, DHTTYPE);

//define pino dos leds
int LEDvermelho = 5;  //pino D1
int LEDverde = 4;     //pino D2

//conexão wifi
const char* ssid = "";
const char* password = "";

//servidor na porta 80
ESP8266WebServer server(80);

float umidade, temperatura;
String valores = "";

unsigned long millisAnterior = 0;  //grava ultima leitura do sensor
const long intervalo = 2000;       //pausa para gravar ultima leitura

void setup() {
  Serial.begin(115200);
  dht.begin();  //inicia sensor

  //inicia led vermelho como desligado
  pinMode(LEDvermelho, OUTPUT);
  digitalWrite(LEDvermelho, LOW);

  //inicia led verde como desligado
  pinMode(LEDverde, OUTPUT);
  digitalWrite(LEDverde, LOW);

  WiFi.begin(ssid, password);  //inicia comunicação wifi

  Serial.println("Verificando estado do sensor: \n\n");
  delay(500);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Tentando conexão");
  }

  Serial.println("");
  Serial.print("Conectado no IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("Conexão efetuada com sucesso!");

  server.begin();
  Serial.print("WebServer inicializado");
  delay(500);

  server.on("/", []() {
    server.send(200, "caebçalho", "Servidor NODEMCU");
  });

  server.on("/onred", []() {
    server.send(200, "cabeçalho", "Led vermelha acessa");
    digitalWrite(LEDvermelho, HIGH);
    delay(1000);
  });

  server.on("/offred", []() {
    server.send(200, "cabeçalho", "Led vermelha apagada");
    digitalWrite(LEDvermelho, LOW);
    delay(1000);
  });

  server.on("/ongreen", []() {
    server.send(200, "cabeçalho", "Led verde acessa");
    digitalWrite(LEDverde, HIGH);
    delay(1000);
  });

  server.on("/offgreen", []() {
    server.send(200, "cabeçalho", "Led verde apagada");
    digitalWrite(LEDverde, LOW);
    delay(1000);
  });

  server.on("/dht11/temp", []() {
    getDados();
    valores = String((int)temperatura);
    server.send(200, "cabeçalho", valores);
  });

  server.on("/dht11/umi", []() {
    getDados();
    valores = String((int)umidade);
    server.send(200, "cabeçalho", valores);
  });
}

void loop() {
  server.handleClient();
}

void getDados() {

  unsigned long millisAtual = millis();

  if (millisAtual - millisAnterior >= intervalo) {
    millisAnterior = millisAtual;

    umidade = dht.readHumidity();
    temperatura = dht.readTemperature();

    if (isnan(umidade) || isnan(temperatura)) {
      Serial.println("Falha na leitura do sensor");

      return;
    }
  }
}