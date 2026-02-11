#include <OneWire.h>  
#include <DallasTemperature.h>  

// ---------------------- Configuração do sensor ----------------------
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ---------------------- Configuração do TRIAC -----------------------
int LAMP = 4;              // Saída para optoacoplador do TRIAC (MOC3021 por ex.)
volatile int dimming = 80; // 6 (máxima potência) até 116 (mínima potência)

// ---------------------- Controle de temperatura ---------------------
int tempref = 10; // Temperatura desejada
float tempc = 0;  // Temperatura atual
int absoluto = 0;
int var = 1;

// ---------------------- Temporização ---------------------
unsigned long lastTempRead = 0;
const unsigned long tempInterval = 1000; // 1s

// ---------------------- Estabilização do sensor ---------------------
int contadorLeituras = 0;
bool sensorEstabilizado = false;

// ---------------------- Setup ----------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("========================================");
  Serial.println("  CONTROLE DE TEMPERATURA COM DS18B20");
  Serial.println("========================================");
  Serial.println();
  
  // Inicia o sensor
  sensors.begin();
  
  // Verifica se o sensor foi encontrado
  int numSensores = sensors.getDeviceCount();
  Serial.print("Sensores encontrados: ");
  Serial.println(numSensores);
  
  if (numSensores == 0) {
    Serial.println("ERRO: Nenhum sensor encontrado!");
    Serial.println("Verifique as conexoes:");
    Serial.println("- GND sensor -> GND Arduino");
    Serial.println("- VCC sensor -> 5V Arduino");
    Serial.println("- DQ sensor -> Pino A0");
    Serial.println("- Resistor 4.7k entre DQ e VCC");
    Serial.println();
    while(1); // Para a execução
  }
  
  Serial.print("Temperatura desejada: ");
  Serial.print(tempref);
  Serial.println(" C");
  Serial.println();
  Serial.println("Aguardando estabilizacao do sensor...");
  Serial.println();
  
  pinMode(LAMP, OUTPUT);
  
  // Interrupção no pino 2 (INT0) — Zero-cross detectado na borda de subida
  attachInterrupt(digitalPinToInterrupt(2), zero_crosss_int, RISING);
}

// ---------------------- Loop -----------------------------
void loop() {
  unsigned long now = millis();
  
  // Faz leitura da temperatura a cada tempInterval
  if (now - lastTempRead >= tempInterval) {
    lastTempRead = now;
    
    sensors.requestTemperatures();
    tempc = sensors.getTempCByIndex(0);
    
    // Verifica se a leitura é válida
    if (tempc == DEVICE_DISCONNECTED_C) {
      Serial.println("ERRO: Falha na leitura do sensor!");
      return;
    }
    
    // Ignora as duas primeiras leituras
    if (!sensorEstabilizado) {
      if (contadorLeituras < 2) {
        Serial.print("Estabilizando... Leitura ");
        Serial.print(contadorLeituras + 1);
        Serial.print(" de 2 (");
        Serial.print(tempc, 1);
        Serial.println(" C - ignorada)");
        contadorLeituras++;
        return;
      } else {
        sensorEstabilizado = true;
        Serial.println("Sensor estabilizado! Iniciando controle...");
        Serial.println();
      }
    }
    
    // Exibe temperatura atual
    Serial.print("Temperatura: ");
    Serial.print(tempc, 1);
    Serial.print(" C | Desejada: ");
    Serial.print(tempref);
    Serial.print(" C | Dimming: ");
    Serial.print(dimming);
    
    // Calcula diferença absoluta
    absoluto = abs((int)tempc - tempref);
    
    // Define variação baseada na diferença
    if (absoluto > 10) {
      var = 5;
    } else if (absoluto > 5) {
      var = 2;
    } else {
      var = 1;
    }
    
    // Controle de temperatura
    if (tempc < tempref) { 
      // Precisa aquecer mais
      dimming -= var;
      if (dimming < 6) dimming = 6; // limite inferior
      Serial.print(" | Aumentando potencia");
    } 
    else if (tempc > tempref) { 
      // Está quente demais
      dimming += var;
      if (dimming > 116) dimming = 116; // limite superior
      Serial.print(" | Diminuindo potencia");
    } else {
      Serial.print(" | Temperatura OK");
    }
    
    Serial.println();
  }
}

// ---------------------- Interrupção de zero-cross --------
void zero_crosss_int() {
  int atraso = (int)(dimming * 65); // 65us ≈ 8,33ms / 128 passos
  delayMicroseconds(atraso);
  digitalWrite(LAMP, HIGH);
  delayMicroseconds(100); // pulso de disparo
  digitalWrite(LAMP, LOW);
}
