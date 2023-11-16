#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>

#define ONE_WIRE_BUS 22 // Pino D22 do ESP32

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

const char* ssid = "Ademilton";
const char* password = "*********";
const char* thingsboardServer = "";
const char* telemetryEndpoint = "/api/v1/********/telemetry";
const char* contentType = "application/json";

WiFiClient client;

bool sendTelemetry(String payload);
void reconnect();

void setup() {
  // Inicialize o Wi-Fi
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  // Aguarde a conexão com o Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  
  sensors.begin();
}

void loop() {
  while (!client.connected()) {
    reconnect();
  }
  
  // Realize a leitura da temperatura
  sensors.requestTemperatures();
  float temperatura = sensors.getTempCByIndex(0);

  // Crie um payload JSON com os dados
  String payload = "{\"temperatura\":" + String(temperatura) + "}";
  Serial.println(payload);
  
  // Envie os dados via HTTP POST
  if (sendTelemetry(payload)) {
    Serial.println("Dados de temperatura enviados para o ThingsBoard com sucesso.");
  } else {
    Serial.println("Falha ao enviar dados de temperatura para o ThingsBoard.");
  }
  
  delay(100);
}

bool sendTelemetry(String payload) {
  if (client.connect(thingsboardServer, 8083)) {
    client.print("POST ");
    client.print(telemetryEndpoint);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(thingsboardServer);
    client.println("Content-Type: " + String(contentType));
    client.print("Content-Length: ");
    client.println(payload.length());
    client.println();
    client.print(payload);

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
      }
    }

    client.stop();
    return true;
  } else {
    Serial.println("Falha na conexão HTTP com o ThingsBoard.");
    client.stop();
    return false;
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconectando ao ThingsBoard...");
    if (client.connect(thingsboardServer, 8080)) {
      Serial.println("Conectado ao ThingsBoard!");
    } else {
      Serial.println("Falha na reconexão com o ThingsBoard. Tentando novamente em 5 segundos...");
      Serial.println(WiFi.localIP());
      delay(5000);
    }
  }
}