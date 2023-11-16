#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define ONE_WIRE_BUS 22 // Pino D22 do ESP32

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "Ademilton"; // Nome da rede Wi-Fi
const char* password = "*********"; // Senha da rede Wi-Fi
const char* mqttServer = ""; // Endereço IP do servidor MQTT (ThingsBoard)
const int mqttPort = 1883; // Porta MQTT padrão
const char* mqttUsername = "testeMQTT"; // Nome de usuário MQTT
const char* mqttPassword = "mqteste"; // Senha MQTT
const char* mqttClient   = "testeMQTT"; // ClientIDMQTT

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  sensors.begin();
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  sensors.requestTemperatures();
  float temperatura = sensors.getTempCByIndex(0);

  String topic = "v1/devices/me/telemetry";
  String payload = "{\"temperatura\":" + String(temperatura) + "}";

  if (client.publish(topic.c_str(), payload.c_str())) {
    Serial.println("Dados de temperatura enviados para o ThingsBoard com sucesso.");
  } else {
    Serial.println("Falha ao enviar dados de temperatura para o ThingsBoard.");
  }

  client.loop();
  delay(10000); // Envie dados a cada 10 segundos
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Conectando ao servidor MQTT...");
    if (client.connect(mqttClient, mqttUsername, mqttPassword)) {
      Serial.println("Conectado ao servidor MQTT!");
    } else {
      Serial.println("Falha na conexão MQTT. Tentando novamente em 5 segundos...");
      delay(5000);
    }
  }
}