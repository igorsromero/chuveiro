#include <OneWire.h>
#include <DallasTemperature.h>

// Define o pino de dados do sensor
#define ONE_WIRE_BUS 2  // Pino digital 2 do Arduino UNO

// Configura a comunicação OneWire
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Contador de leituras
int contadorLeituras = 0;

void setup() {
  // Inicia a comunicação serial
  Serial.begin(9600);
  Serial.println("=============================");
  Serial.println("  Teste Sensor DS18B20");
  Serial.println("=============================");
  Serial.println();
  
  // Inicia o sensor
  sensors.begin();
  
  // Verifica quantos sensores foram encontrados
  int numSensores = sensors.getDeviceCount();
  Serial.print("Sensores encontrados: ");
  Serial.println(numSensores);
  Serial.println();
  
  if (numSensores == 0) {
    Serial.println("ERRO: Nenhum sensor encontrado!");
    Serial.println("Verifique as conexoes:");
    Serial.println("- GND do sensor -> GND do Arduino");
    Serial.println("- VCC do sensor -> 5V do Arduino");
    Serial.println("- DQ do sensor -> Pino 2 do Arduino");
    Serial.println("- Resistor 4.7k entre DQ e VCC");
    Serial.println();
    while(1); // Para a execução
  }
  
  Serial.println("Aguardando estabilizacao do sensor...");
  Serial.println();
}

void loop() {
  // Solicita a leitura da temperatura
  sensors.requestTemperatures();
  
  // Lê a temperatura em Celsius
  float temperatureC = sensors.getTempCByIndex(0);
  
  // Ignora as duas primeiras leituras
  if (contadorLeituras < 2) {
    Serial.print("Estabilizando... Leitura ");
    Serial.print(contadorLeituras + 1);
    Serial.print(" de 2");
    Serial.print(" (");
    Serial.print(temperatureC, 1);
    Serial.println(" °C - ignorada)");
    
    contadorLeituras++;
    delay(2000);
    return; // Pula o resto do código
  }
  
  // Verifica se a leitura é válida
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    Serial.println("-----------------------------");
    Serial.print("Temperatura: ");
    Serial.print(temperatureC, 1);
    Serial.println(" °C");
    Serial.println("-----------------------------");
  } else {
    Serial.println("ERRO: Falha na leitura do sensor!");
  }
  
  // Aguarda 2 segundos antes da próxima leitura
  delay(2000);
}
