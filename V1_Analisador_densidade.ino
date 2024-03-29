/*    DENSIMETRO DIGITAL AUTOMATIZADO
 PROJETO INTEGRADOR II: PROTÓTIPO DE PROTOTIPO PARA ENGENHARIA ELETRICA
GRUPO: GABRIEL BASSI ZILLIG
*/

//  DECLARAÇÃO DE BIBLIOTECAS
#include "HX711.h"
#include <Ultrasonic.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LCD5110_Basic.h>
#include <EEPROM.h>
#define DOUT  3
#define CLK  2
#define ONE_WIRE_BUS 8
#define TRIGGER_PIN  12
#define ECHO_PIN     13
Ultrasonic ultrasonic(TRIGGER_PIN, ECHO_PIN);
HX711 scale(DOUT, CLK);
LCD5110 myGLCD(14,15,16,18,17); //CLK, DIN, DC, RST, CE
extern uint8_t SmallFont[];
extern uint8_t BigNumbers[];
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//  DECLARAÇÃO DE VARIÁVEIS GLOBAIS
float cmMsec;
long microsec;
float peso;
float temp_sensor;
float temp;
float calibration_factor_base = -374.5;
int calibration_memoria;
float calibration_memoria_float;
float calibration_factor;
float calibration_memoria_temp;
//  DECLARAÇÃO DE PINOS
char botao_cima = 4;
char botao_baixo = 5;
char botao_ent = 6;
char num_menu = 1 ;
char dreno = 9;
char bomba_encher = 11;
long ref_nivel;
char rodando_analise=LOW;
int tela_impressa;

void setup() {
// DECLARAÇÃO DE TIPOS DE I/O
pinMode(botao_cima, INPUT_PULLUP);
pinMode(botao_baixo, INPUT_PULLUP);
pinMode(botao_ent, INPUT_PULLUP); 
pinMode(dreno, OUTPUT);
digitalWrite (dreno, HIGH);
pinMode(bomba_encher, OUTPUT);
digitalWrite (bomba_encher, HIGH);
Serial.begin(9600);
myGLCD.InitLCD();
ref_nivel = EEPROM.read(3);
calibration_memoria = EEPROM.read(2);
calibration_memoria_temp = EEPROM.read(1);
calibration_factor = calibration_factor_base + (calibration_memoria/10);
scale.set_scale(calibration_factor); 
scale.tare();
sensors.begin();
Serial.begin(9600);
Serial.println("SERIAL OK");
}
// ============================================== S U B - R O T I N A S =================================================




int ler_botao()  //FUNÇÃO: IDENTIFICA E RETORNA O BOTÃO DO TECLADO QUE FOI PRESSIONADO
{ 
  int botao = 0;
  // LEMBRANDO QUE OS BOTÕES SEGUEM LOGICA REVERSA (QUANDO PRESSIOANDOS, FICAM LOW)
  // ESSE CÓDIGO PROCURA CONTORNAR PROBLEMAS POR TREPIDAÇÃO MECÂNICA DOS BOTÕES. PARA ISSO, USA A FUNÇÃO DELAY
    if (digitalRead(botao_ent) == LOW)
  {
    delay(50);
    if (digitalRead(botao_ent) == LOW)
    {
      botao = 1;
    }
  }


  if (digitalRead(botao_cima) == LOW)
  {
    delay(50);
    if (digitalRead(botao_cima) == LOW)
    {
      botao = 2;
    }
  }
  
  
 if (digitalRead(botao_baixo) == LOW)
  {
    delay(50);
    if (digitalRead(botao_baixo) == LOW)
    {
      botao = 3;
    }
  }

  return botao;
}



void menu()
{
  switch (ler_botao())
  {
    case 2:
    num_menu++;
    if (num_menu > 10)
    {
      num_menu = 1;
    }
    break;

    case 3:
    num_menu--;
     if (num_menu < 1)
    {
      num_menu = 10;
    }
    break;
  }

  switch (num_menu)
  {
    case 1:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("ANALISADOR", CENTER, 15);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
    if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    analisador();
    }
    break;

    case 2:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("CALIBRAR", CENTER, 0);
    myGLCD.print("BALANCA", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
    if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    balanca_cal();
    }
    break;
    
    case 3:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("CALIBRAR", CENTER, 0);
    myGLCD.print("TERMOMETRO", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
     if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    temp_cal();
    }
    break;
    
    case 4:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("DRENAR", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
      if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    drenar();
    }
    break;
    
    case 5:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("ENCHER", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
     if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    encher();
    }
    break;

    case 6:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("LIMPAR", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
     if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    limpar();
    }
    break;

    case 7:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("TARA", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
    if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    tara();
    }
    break;

    case 8:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("LEITURA", CENTER, 0);
    myGLCD.print("DOS SENSORES", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
    if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    leituras();
    }
    break;

    case 9:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("SOBRE /", CENTER, 0);
    myGLCD.print("AUTORES", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
    if (ler_botao() == 1)
    {
    sobre();
    }
    break;

    case 10:
    tela_impressa=0;
    if (tela_impressa==0){
    myGLCD.setFont(SmallFont);
    myGLCD.print("CALIBRAR", CENTER, 0);
    myGLCD.print("NIVEL", CENTER, 10);
    myGLCD.print("ENTER: SELECT", CENTER, 35);
    myGLCD.print("SETAS: TOOLS", CENTER, 40);
    tela_impressa=1;
    }
    if (ler_botao() == 1)
    {
    myGLCD.clrScr();
    nivel_cal();
    }
    break;
  }
  myGLCD.clrScr();
}

void balanca_cal()
{
   /*myGLCD.clrScr(); 
    myGLCD.print("EXECUTANDO", CENTER, 10);
    myGLCD.print("TARA", CENTER, 20);
    delay(2000);
    scale.tare();*/
  while (num_menu == 2) {
 peso = scale.get_units();
 myGLCD.clrScr(); 
 myGLCD.print("Peso:", LEFT, 0);
 myGLCD.printNumF(peso, 2, RIGHT, 0);
 myGLCD.print("Fator:", LEFT, 15);
 myGLCD.printNumF(calibration_factor, 1, RIGHT, 15);
 delay(50);
 switch (ler_botao())
  {
    case 1: 
    myGLCD.clrScr(); 
    myGLCD.print("Salvando...", CENTER, 0);
    EEPROM.write(2, calibration_memoria);
    delay(2000);
    num_menu = 1;
    break;
  
    case 2:
    calibration_memoria++;
    break;

    case 3:
    calibration_memoria--;
    break;
  }
  calibration_memoria_float = calibration_memoria;
  calibration_factor = calibration_factor_base + (calibration_memoria_float/10);
 scale.set_scale(calibration_factor); 
  }
}

void analisador()
{
  float densidade=0;
  float leituras_peso[] = {0, 0, 0, 0, 0};
  float leituras_temp[] = {0, 0, 0, 0, 0};
  float media_peso=0;
  float media_temp=0;
  int resultado = LOW;
  rodando_analise = HIGH;
  limpar();
  tara();
  encher();
  delay(1000);
  myGLCD.clrScr();
  myGLCD.print("Aguardando", CENTER, 0);
  myGLCD.print("Histerese...", CENTER, 10);
  delay(30000);
  
  for (int i=0; i < 5; i++)  //REALIZA 5 LEITURAS DE TEMP E PESO, COM 10s DE INTERVALO.
  {
  delay(10000);
  myGLCD.clrScr();
  myGLCD.print("Realizando", CENTER, 0);
  myGLCD.print("Leitura:", LEFT, 10);
  myGLCD.printNumF(i, 0, RIGHT, 10);
  sensors.requestTemperatures();
  peso = scale.get_units();
  temp_sensor = sensors.getTempCByIndex(0);
  temp = temp_sensor + (calibration_memoria_temp / 10);
  leituras_peso[i] = peso;
  leituras_temp[i] = temp;
  }

  for (int i=0; i < 5; i++)  //CÁLCULO DAS MÉDIAS DE LEITURA
  {
    media_peso = media_peso + leituras_peso[i];
    media_temp = media_temp + leituras_temp[i];
  }
  media_peso = media_peso / 5;
  media_temp = media_temp / 5;
  densidade = media_peso / 250;
  resultado = HIGH;
  myGLCD.clrScr();
  
  while (resultado == HIGH)
  {
    myGLCD.setFont(SmallFont);
    myGLCD.print("Peso:", LEFT, 0);
    myGLCD.printNumF(media_peso, 2, RIGHT, 0);
    myGLCD.print("Temp (c):", LEFT, 10);
    myGLCD.printNumF(media_temp, 2, RIGHT, 10);
    myGLCD.print("DENSIDADE:", CENTER, 20);
    myGLCD.printNumF(densidade, 3, LEFT, 30);
    myGLCD.print("g/cm3", RIGHT, 30);
    myGLCD.print("ENT p/ drenar", LEFT, 40);
    if (ler_botao() == 1)
    {
      resultado = LOW;
    }
  }
  drenar();
  rodando_analise = LOW;
}

void temp_cal()
{
   while (num_menu == 3) {
    
    myGLCD.setFont(SmallFont);
    myGLCD.clrScr();
    myGLCD.print("Temp (c):", LEFT, 0);
    myGLCD.printNumF(temp, 2, RIGHT, 0);
    myGLCD.print("Fator:", LEFT, 15);
    myGLCD.printNumF(calibration_memoria_temp, 0, RIGHT, 15);
     switch (ler_botao())
      {
        case 1: 
        myGLCD.clrScr(); 
        myGLCD.print("Salvando...", CENTER, 0);
        EEPROM.write(1, calibration_memoria_temp);
        delay(2000);
        num_menu = 1;
        break;
      
        case 2:
        calibration_memoria_temp = calibration_memoria_temp + 1;
        break;
    
        case 3:
        calibration_memoria_temp = calibration_memoria_temp - 1;
        break;
      }
   sensors.requestTemperatures();
   temp_sensor = sensors.getTempCByIndex(0);
   temp = temp_sensor + (calibration_memoria_temp / 10);
  }
}

void nivel_cal()
{
  while (num_menu == 10){
     myGLCD.setFont(SmallFont);
    myGLCD.clrScr();
    myGLCD.print("Pres. ENTER", CENTER, 0);
    myGLCD.print("P/ GRAVAR NIVEL", CENTER, 10);
    microsec = ultrasonic.timing();
    myGLCD.print("Nivel:", LEFT , 30);
    myGLCD.printNumF(microsec, 0, RIGHT , 30);
    myGLCD.print("Setpoint:", LEFT , 40);
    ref_nivel = EEPROM.read(3);
    myGLCD.printNumF(ref_nivel, 0, RIGHT , 40);
    delay(50);
    if (ler_botao() == 1)
    {
       myGLCD.clrScr(); 
       myGLCD.print("Salvando...", CENTER, 0);
       EEPROM.write(3, microsec);
       delay(2000);
       num_menu = 1;
    }
  }
}

void drenar()
{
    int okparadrenar = 0;
    myGLCD.clrScr();
    while (okparadrenar == 0)
  { 
    myGLCD.print("PREPARAR", CENTER, 0);
    myGLCD.print("SAIDA", CENTER, 10);
    myGLCD.print("Pres. ENTER", CENTER, 25);
    if (ler_botao() == 1)
    {
      okparadrenar = 1;
    }
  }
  myGLCD.clrScr(); 
  myGLCD.print("DRENANDO...", CENTER, 0);
  digitalWrite (dreno, LOW);
  delay (30000);
  digitalWrite (dreno, HIGH);
  myGLCD.clrScr();
  myGLCD.print("CONCLUIDO", CENTER, 0);
  delay(2000);
  if ( rodando_analise == LOW){
  num_menu = 1;
  }
}

void encher()
{
    
    int okparaencher = 0;
    myGLCD.clrScr();
    while (okparaencher == 0)
  { 
    myGLCD.print("PREPARAR", CENTER, 0);
    myGLCD.print("COLETA", CENTER, 10);
    myGLCD.print("Pres. ENTER", CENTER, 25);
    if (ler_botao() == 1)
    {
      microsec = ultrasonic.timing(); 
      delay(500);
      if (microsec < 500){                //checagem para saber se já esta cheio
        okparaencher = 0;
        myGLCD.clrScr();  
       myGLCD.print("CHEIO", CENTER, 0);
       delay(2000);
       drenar();
      }
      else{                           // se estiver vazio
      okparaencher = 1;
      myGLCD.clrScr();  
      myGLCD.print("ENCHENDO...", CENTER, 0);
       digitalWrite(bomba_encher, LOW);
      }
    }
  }
 int enchendo = 1;
 while (enchendo == 1){
    microsec = ultrasonic.timing();
    delay(60);
    
   if (microsec < (ref_nivel+40)){
    delay(1000);
        digitalWrite(bomba_encher, HIGH);
        delay(1000);
        myGLCD.clrScr();
        myGLCD.print("CONCLUIDO", CENTER, 0);
        delay(2000);
        enchendo = 0;
    }
  }
  
  if (rodando_analise == LOW){
  num_menu = 1;
  }
}

void limpar()
{
    
    int okparaencher = 0;
    myGLCD.clrScr();
    while (okparaencher == 0)
  { 
    myGLCD.print("PREPARAR", CENTER, 0);
    myGLCD.print("COLETA", CENTER, 10);
    myGLCD.print("Pres. ENTER", CENTER, 25);
    if (ler_botao() == 1)
    {
      microsec = ultrasonic.timing();
      delay(500);
      if (microsec < 200){
        okparaencher = 0;
        myGLCD.clrScr();  
       myGLCD.print("CHEIO", CENTER, 0);
       delay(2000);
       drenar();
      }
      else{
      okparaencher = 1;
      myGLCD.clrScr();  
      myGLCD.print("ENCHENDO...", CENTER, 0);
       digitalWrite(bomba_encher, LOW);
      }
    }
  }
  

    delay(7000);
    digitalWrite(bomba_encher, HIGH);
        myGLCD.clrScr();
        myGLCD.print("CONCLUIDO", CENTER, 0);
        okparaencher = 0;
         delay(1000);
         
    int okparadrenar = 0;
    myGLCD.clrScr();
    while (okparadrenar == 0)
  { 
    myGLCD.print("PREPARAR", CENTER, 0);
    myGLCD.print("SAIDA", CENTER, 10);
    myGLCD.print("Pres. ENTER", CENTER, 25);
    if (ler_botao() == 1)
    {
      okparadrenar = 1;
    }
  }
  myGLCD.clrScr(); 
  myGLCD.print("DRENANDO...", CENTER, 0);
  digitalWrite (dreno, LOW);
  delay (18000);
  digitalWrite (dreno, HIGH);
  myGLCD.clrScr();
  myGLCD.print("CONCLUIDO", CENTER, 0);
  delay(2000);
  if ( rodando_analise == LOW){
  num_menu = 1;
  }   
}
void tara()
{
   int tara_executada = 0;
   myGLCD.clrScr(); 
    while(tara_executada == 0)
  {
    myGLCD.print("Pres. ENTER", CENTER, 0);
    myGLCD.print("p/ executar", CENTER, 10);
    myGLCD.print("TARA", CENTER, 20);
    if (ler_botao() == 1)
    {
      tara_executada = 1;
      scale.tare();
      myGLCD.clrScr(); 
      myGLCD.print("CONCLUIDO", CENTER, 10);
      delay(1000);
    }
  }
  if (rodando_analise == LOW){
  num_menu = 1;
  }
}

void leituras()
{
  while (num_menu == 8)
  {
    sensors.requestTemperatures();
    peso = scale.get_units();
    temp_sensor = sensors.getTempCByIndex(0);
    temp = temp_sensor + (calibration_memoria_temp / 10);
    microsec = ultrasonic.timing();
    cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
    digitalWrite(bomba_encher, HIGH);
    myGLCD.setFont(SmallFont);
    myGLCD.clrScr();
    myGLCD.print("Peso:", LEFT, 0);
    myGLCD.printNumF(peso, 2, RIGHT, 0);
    myGLCD.print("Temp (c):", LEFT, 10);
    myGLCD.printNumF(temp, 2, RIGHT, 10);
    myGLCD.print("Nivel:", LEFT , 20);
    myGLCD.printNumF(cmMsec, 2, RIGHT , 20);
    myGLCD.print("+ p/ sair", CENTER, 40);
    if (ler_botao() == 2)
    {
    num_menu = 1;
    }
    delay(50);  
  }
}
void sobre()
{
  myGLCD.clrScr();
  while (num_menu == 9)
  {
   myGLCD.setFont(SmallFont);
    myGLCD.print("AUTOR:", CENTER, 0);
    myGLCD.print("Gabriel B. Z.", CENTER, 20);
    myGLCD.print("Eng. UNISA", CENTER, 40);
     if (ler_botao() == 2)
    {
    num_menu = 1;
    }
  }
}
// ================================================  FIM DAS S U B - R O T I N A S
void loop() {
 menu();
 
}
