#include <OneWire.h>  //INCLUDE DE BIBLIOTECA DO SENSOR DE TEMPERATURA.
#include <DallasTemperature.h>  //INCLUDE DE BIBLIOTECA DO SENSOR DE TEMPERATURA.

#define ONE_WIRE_BUS 13 //DEFINIÇÃO DA PORTA DE ENTRADA DO SENSOR DE TEMPERATURA.
OneWire oneWire(ONE_WIRE_BUS);  //CONFIGURANDO A INSTANCIA oneWire PARA SE COMUNICAR COM QUALQUER DISPOSITIVO OneWire.
DallasTemperature sensors(&oneWire);  //PASSANDO A REFERENCIA oneWire PARA A BIBLIOTECA DALLAS TEMPERATURE.


int LAMP = 13; //SAÍDA PARA O OPTOACOPLADOR
int dimming = 6; //DIMMING 0(ON) - 128(OFF); ESTÁVEL --> 6(ON) 116(OFF). 
int tempref=30; // TEMPERATURA DESEJADA.
int tempc = 0;  //TEMPERATURA ATUAL.


void setup()
{
  Serial.begin(9600); //INICIA COMUNICAÇÃO SERIAL.
  Serial.println("MONITORAMENTO DIGITAL DE TEMPERATURA"); //MOSTRA A MENSAGEM.
  sensors.begin();  //INICIA O SENSOR DE TEMPERATURA.
  pinMode(LAMP, OUTPUT);  //INDICA QUE A VARIÁVEL LAMP(3) É UMA SAÍDA.
  attachInterrupt(0, zero_crosss_int, RISING); //CHAMA A FUNÇÃO ZERO_CROSS_INT NA BORDA DE SUBIDA.
}


void loop()
{
 sensors.requestTemperatures(); //FAZ REQUISIÇÃO DA TEMPERATURA PARA O SENSOR DE TEMPERATURA.
 Serial.print("Temperatura: ");  //MOSTRA A MENSAGEM.
 tempc = sensors.getTempCByIndex(0);  //ARMAZENA O VALOR DA TEMPERATURA NA VARIAVEL TEMPC
 Serial.println(tempc);  //MOSTRA A TEMPERATURA

if(tempc<tempref){   //SE A TEMPERATURA ATUAL FOR MENOR QUE A TEMPERATURA DESEJADA
  if (dimming>5){ //TRATAMENTO NECESSÁRIO PRA NÃO LEVAR O DIMMING PRA NEGATIVO.
    dimming = dimming-1;  //DIMMING DIMINUI 1 - AUMENTA O ÂNGULO DE FASE DA ONDA SENOIDAL
    Serial.print("Aumentando.....");  //MOSTRA MENSAGEM
    }
  delay(200);
  }
  
if(tempc>tempref){  //SE A TEMPERATURA ATUAL FOR MAIOR QUE A TEMPERATURA DESEJADA
  if (dimming<117){ //TRATAMENTO NECESSÁRIO PARA NÃO LEVAR O DIMMING PRA UM VALOR MAIOR QUE 126
    dimming = dimming+1;  //DIMMING AUMENTA 1 - DIMINUI O ÂNGULO DE FASE DA ONDA SENOIDAL
    Serial.print("Diminuindo.....");  //MOSTRA MENSAGEM
    }
  delay(200);
  }
  
  Serial.print("Dimming="); //MOSTRA MENSAGEM
  Serial.println(dimming,DEC);  //VALOR DO DIMMING EM DECIMAL
  delay(100);
}


void zero_crosss_int()
{
  //encontrando o angulo para 60Hz:
  //temos que 1 ciclo = 1/60 = 0.01666...(s); logo,
  //1/2 ciclo = 0.01666/2 = 0.008333...(s) = 8.33ms; (meio ciclo)
  //(8333us - 8.33us)/128 ~= 65 = ÂNGULO PARA A FREQUÊNCIA DE 60Hz
  int dimtime = (65 * dimming); //VARIAVEL QUE DA O ANGULO PRA ONDA
  //TEMPO DE ESPERA (ÂNGULO)
  
  //para 128 -> dimming = 8.33ms = 0%
  //para 64  -> dimming = 50%
  //para 0 -> dimming = 100%
  delayMicroseconds(dimtime); //DELAY DO ANGULO
  digitalWrite(LAMP, HIGH);
  delayMicroseconds(8.33); //DELAY PARA ESTABILIZAR 
  digitalWrite(LAMP, LOW);
}
