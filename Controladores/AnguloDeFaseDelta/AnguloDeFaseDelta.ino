#include <OneWire.h>  
#include <DallasTemperature.h>  

// ---------------------- Configuração do sensor ----------------------
#define ONE_WIRE_BUS A0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ---------------------- Configuração do TRIAC -----------------------
int LAMP = 4;              // Saída para optoacoplador do TRIAC (MOC3021 por ex.)
volatile int dimming = 80; // 6 (máxima potência) até 116 (mínima potência)

// ---------------------- Controle de temperatura ---------------------
int tempref = 60; // Temperatura desejada
int tempc = 0;    // Temperatura atual
int absoluto = 0;
int var = 1;

// ---------------------- Temporização ---------------------
unsigned long lastTempRead = 0;
const unsigned long tempInterval = 1000; // 1s

// ---------------------- Setup ----------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("MONITORAMENTO DIGITAL DE TEMPERATURA");
  
  sensors.begin();
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
    
    Serial.print("Temperatura: ");
    Serial.println(tempc);

    absoluto = abs(tempc - tempref);
    if (absoluto > 10) {
      var = 5;
    } else if (absoluto > 5) {
      var = 2;
    } else {
      var = 1;
    }

    if (tempc < tempref) { // precisa aquecer mais
      dimming -= var;
      if (dimming < 6) dimming = 6; // limite inferior
      Serial.print("Aumentando potência -> dimming: ");
      Serial.println(dimming);
    } 
    else if (tempc > tempref) { // está quente demais
      dimming += var;
      if (dimming > 116) dimming = 116; // limite superior
      Serial.print("Diminuindo potência -> dimming: ");
      Serial.println(dimming);
    }
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
