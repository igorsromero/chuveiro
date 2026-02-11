#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Define o pino de dados do sensor
#define ONE_WIRE_BUS 13  // GPIO13 do ESP32

// Configuração do display OLED Heltec
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET 16    // Pino RST do Heltec
#define OLED_SDA 4       // Pode precisar ajustar
#define OLED_SCL 15      // Pode precisar ajustar

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Configura a comunicação OneWire
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Contador de leituras
int contadorLeituras = 0;

void setup() {
  // Inicia a comunicação serial
  Serial.begin(115200);
  Serial.println("Iniciando teste do sensor DS18B20...");
  
  // Inicia o display
  Wire.begin(OLED_SDA, OLED_SCL);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("Falha ao inicializar o display!");
    for(;;);
  }
  
  // Tela de inicialização
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 20);
  display.println("DS18B20");
  display.setTextSize(1);
  display.setCursor(20, 45);
  display.println("Iniciando...");
  display.display();
  delay(2000);
  
  // Inicia o sensor
  sensors.begin();
  
  // Verifica quantos sensores foram encontrados
  int numSensores = sensors.getDeviceCount();
  Serial.print("Sensores encontrados: ");
  Serial.println(numSensores);
  
  if (numSensores == 0) {
    Serial.println("ERRO: Nenhum sensor encontrado!");
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(35, 20);
    display.println("ERRO!");
    display.setCursor(5, 40);
    display.println("Sensor nao encontrado");
    display.display();
    while(1); // Para a execução
  }
}

void loop() {
  // Solicita a leitura da temperatura
  sensors.requestTemperatures();
  
  // Lê a temperatura em Celsius
  float temperatureC = sensors.getTempCByIndex(0);
  
  // Limpa o display
  display.clearDisplay();
  
  // Ignora as duas primeiras leituras
  if (contadorLeituras < 2) {
    Serial.print("Aguardando estabilização... Leitura ");
    Serial.print(contadorLeituras + 1);
    Serial.println(" de 2");
    
    // Mostra no display que está aguardando
    display.setTextSize(1);
    display.setCursor(15, 20);
    display.println("Aguardando");
    display.setCursor(15, 35);
    display.println("estabilizacao...");
    display.setCursor(35, 50);
    display.print(contadorLeituras + 1);
    display.print(" de 2");
    display.display();
    
    contadorLeituras++;
    delay(2000);
    return; // Pula o resto do código
  }
  
  // Verifica se a leitura é válida
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperatura: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
    
    // Título
    display.setTextSize(1);
    display.setCursor(20, 5);
    display.println("Temperatura");
    
    // Temperatura grande
    display.setTextSize(3);
    display.setCursor(10, 25);
    display.print(temperatureC, 1);
    display.setTextSize(2);
    display.print(" C");
    
  } else {
    Serial.println("ERRO: Falha na leitura!");
    
    // Exibe erro no display
    display.setTextSize(2);
    display.setCursor(30, 20);
    display.println("ERRO!");
    display.setTextSize(1);
    display.setCursor(10, 45);
    display.println("Falha na leitura");
  }
  
  display.display();
  
  // Aguarda 2 segundos antes da próxima leitura
  delay(2000);
}
