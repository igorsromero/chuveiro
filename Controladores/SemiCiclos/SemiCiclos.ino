#include <OneWire.h>  //INCLUDE DE BIBLIOTECA DO SENSOR DE TEMPERATURA.
#include <DallasTemperature.h>  //INCLUDE DE BIBLIOTECA DO SENSOR DE TEMPERATURA.
#define ONE_WIRE_BUS A0 //DEFINIÇÃO DA PORTA DE ENTRADA DO SENSOR DE TEMPERATURA.
OneWire oneWire(ONE_WIRE_BUS);  //CONFIGURANDO A INSTANCIA oneWire PARA SE COMUNICAR COM QUALQUER DISPOSITIVO OneWire.
DallasTemperature sensors(&oneWire);  //PASSANDO A REFERENCIA oneWire PARA A BIBLIOTECA DALLAS TEMPERATURE.


int LAMP = 13; //SAÍDA PARA O OPTOACOPLADOR
int dimming = 20; //DIMMING 0(ON) - 128(OFF); ESTÁVEL --> 6(ON) 116(OFF).
int tempref=33; // TEMPERATURA DESEJADA.
int tempc = 0;  //TEMPERATURA ATUAL.
int nct = 120;  //NÚMERO DE SEMI CICLOS TOTAIS.
int ncp = nct/2;  //NÚMERO DE SEMI CICLOS POSITIVOS, OU SEJA, CARREGADOS (INICIA COM 50% DE POTÊNCIA);
int c1 = 1; //CONTADOR PARA QUANTIDADE DE CICLOS.
int c2 = 0; //CONTADOR PARA MEDIÇÃO DE TEMPERATURA.


void setup()
{
  Serial.begin(9600); //INICIA COMUNICAÇÃO SERIAL.
  Serial.println("MONITORAMENTO DIGITAL DE TEMPERATURA"); //MOSTRA A MENSAGEM.
  sensors.begin();  //INICIA O SENSOR DE TEMPERATURA.
  pinMode(LAMP, OUTPUT);  //INDICA QUE A VARIÁVEL LAMP(4) É UMA SAÍDA.
  attachInterrupt(0, zero_crosss_int, RISING); //CHAMA A FUNÇÃO ZERO_CROSS_INT NA BORDA DE SUBIDA.
}


void loop()
{
  if(c2>1)
  {
    sensors.requestTemperatures(); //FAZ REQUISIÇÃO DA TEMPERATURA PARA O SENSOR DE TEMPERATURA.
    Serial.print("Temperature is: ");  //MOSTRA A MENSAGEM.
    tempc = sensors.getTempCByIndex(0);  //ARMAZENA O VALOR DA TEMPERATURA NA VARIAVEL TEMPC.
    Serial.println(tempc);  //MOSTRA A TEMPERATURA.
   
  if(tempc<tempref)
  {   //SE A TEMPERATURA ATUAL FOR MENOR QUE A TEMPERATURA DESEJADA.
    if (ncp<nct){ //TRATAMENTO NECESSÁRIO PARA NÃO LEVA O NCP PARA UM VALOR MAIOR QUE NCT.
      ncp = ncp+1;  //CICLOS POSITIVOS AUMENTA 1.
      Serial.print("Aumentando.....");  //MOSTRA MENSAGEM.
      }
      delay(200);
  }
  
  if(tempc>tempref)
  {  //SE A TEMPERATURA ATUAL FOR MAIOR QUE A TEMPERATURA DESEJADA.
    if (ncp>0){ //TRATAMENTO NECESSÁRIO PARA NÃO LEVAR O NCP PARA UM VALOR NEGATIVO.
      ncp = ncp-1;  //CICLOS POSITIVOS DIMINUI 1.
      Serial.print("Diminuindo.....");  //MOSTRA MENSAGEM.
      }
      delay(200);
  }
  delay(100);
  }
  c2=0; //CONTADOR C2 RECEBE 0.
}


void zero_crosss_int()
{
  if(c1 == nct) //SE O CONTADOR C1 FOR IGUAL A QUANTIDADE DE CICLOS TOTAIS.
  {
    c1 = 0; //CONTADOR C1 RECEBE 0.
    c2 = c2 + 1;  //CONTADOR C2 AUMENTA 1.
    }else {
      if(c1 < ncp)  //SE CONTADOR C1 FOR MENOR A QUANTIDADE DE CICLOS POSITIVOS.
      {
        delayMicroseconds(390); //DELAY PARA LIGADO.
        digitalWrite(LAMP, HIGH);
        delayMicroseconds(8.33);
        digitalWrite(LAMP, LOW);
      }
      c1 = c1 + 1;  //CONTADOR C1 AUMENTA 1.
    }
}
