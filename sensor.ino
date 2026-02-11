#include <OneWire.h>
#include <DallasTemperature.h>

// Define o pino de dados do sensor
#define ONE_WIRE_BUS 16  // GPIO4 do ESP32

// Configura a comunicação OneWire
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  // Inicia a comunicação serial
  Serial.begin(115200);
  Serial.println("Iniciando teste do sensor DS18B20...");
  
  // Inicia o sensor
  sensors.begin();
  
  // Verifica quantos sensores foram encontrados
  int numSensores = sensors.getDeviceCount();
  Serial.print("Sensores encontrados: ");
  Serial.println(numSensores);
  
  if (numSensores == 0) {
    Serial.println("ERRO: Nenhum sensor encontrado! Verifique as conexões.");
  }
}

void loop() {
  // Solicita a leitura da temperatura
  sensors.requestTemperatures();
  
  // Lê a temperatura em Celsius
  float temperatureC = sensors.getTempCByIndex(0);
  
  // Verifica se a leitura é válida
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperatura: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
  } else {
    Serial.println("ERRO: Falha na leitura do sensor!");
  }
  
  // Aguarda 2 segundos antes da próxima leitura
  delay(2000);
}
