// NOTA: se debe cargar dos veces este programa 1. Con la linea  RTC.adjust(DateTime(__DATE__, __TIME__));
//                                              2. Sin esa linea
///////////////////////////////////////////////////////////////////////////////////////////////////////
//                        PROGRAMA DISEÑADO PARA LA PRUEBA DEL CRONOGRAMA
//             EL CUAL REALIZA COMBIOS RAPIDOS DE ESTADO, ASÍ VER SÍ FUNCIONA EL PROGRAMA
///////////////////////////////////////////////////////////////////////////////////////////////////////
////LIBRERIAS A UTILIZAR
#include <Wire.h>
#include "RTClib.h"
//inicializa pantalla lcd
#include <LiquidCrystal.h>
#include <DHT.h>
#include <EEPROM.h>
// inicializa el modulo RTC
RTC_DS1307 RTC;

//////CONFIGURACIÓN PINES DE KEYPAD SHIELD /////////////
//LCD pin to Arduino
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
//////CONFIGURACIÓN DE PIN DE LECTURA DHT11/////////
#define  pinDH  34
#define DHTTYPE DHT11
DHT dht(pinDH, DHTTYPE);
// PIN 30 ENCARGADO DE RECIBIR DATOS DE DHT11
/////////////SENSOR DE HUMEDAD EN EL SUELO///////////////
#define Hsuelo A9
//////CONFIGURACIÓN DE PINES DE SALIDA PARA LUCES LED/////
#define ledM4  25             // PIN 32 ENCARGADO DE ACTIVAR LAS LUCES LED CONECTADAS AL RELE
#define ledM3  24             // PIN 33 ENCARGADO DE ACTIVAR LAS LUCES LED CONECTADAS AL RELE
#define ledM2  23             // PIN 34 ENCARGADO DE ACTIVAR LAS LUCES LED CONECTADAS AL RELE
#define ledM1  22             // PIN 35 ENCARGADO DE ACTIVAR LAS LUCES LED CONECTADAS AL RELE
#define LEDHUMEDAD 28
#define LEDCO2     29
#define LEDTEMPERATURA  30
#define LEDESTADO0 32
#define LEDINTENSIDAD   31

////////CONFIGURACIÓN DE PINES DE SALIDA PARA BOMBA DE RIEGO Y VENTILADORES//////////
#define pinBomba  52              // PIN 37 ENCARGADO DE ACTIVAR LA BOMBA DE RIEGO
int tiempoRiego = 10000;          //Tiempo continuo que dura la bomba de riego encendida => 10 segundos
#define pinVentilador  53         // PIN 38 ENCARGADO DE ACTIVAR EL VENTILADOR
int tiempoVentilacion = 1500;      //Tiempo continuo que duran los ventiladores encendidos => 15 segundos
//////CONFIGURACIÓN PARA PIN DE LECTURA MQ//////////////////////
#define MQ2_PIN  A10              // Pin del sensor
const int RL_VALUE = 15;          // Resistencia RL del modulo en Kilo ohms
const int R0 = 10;                // Resistencia R0 del sensor en Kilo ohms
// Datos para lectura multiple
const int READ_SAMPLE_INTERVAL = 150;    // Tiempo entre muestras
const int READ_SAMPLE_TIMES = 12;         // Numero muestras
// Ajustar estos valores para vuestro sensor según el Datasheet
// DATOS AJUSTADOS PARA MEDICIÓN DE CO EN EL DATASHEET
const float X0 = 200;
const float Y0 = 5.1;
const float X1 = 10000;
const float Y1 = 1.5;
// Puntos de la curva de concentración {X, Y}
const float punto0[] = { log10(X0), log10(Y0) };
const float punto1[] = { log10(X1), log10(Y1) };
// Calcular pendiente y coordenada abscisas
const float scope = (punto1[1] - punto0[1]) / (punto1[0] - punto0[0]);
const float coord = punto0[1] - punto0[0] * scope;
//////CONFIGURACIÓN PARA LECTURA DE BOTONES EN LA SHIELD////
int estado;
int paso;
////LEDs
int luzMinima = 0;            //VALOR MINIMO ACEPTABLE DE LUZ AMBIENTE SEGÚN EL ESTADO EN QUE ESTE
int luzMaxima = 0;            //VALOR MAXIMO NO PERJUDICIAL DE LUZ AMBIENTE SEGÚN EL ESTADO EN QUE ESTE
////RTC
int segundo;
int minuto;
int hora;
/////////////BANDERAS PARA LA LECTURA O ESCRITURA/////////////
boolean button ;
boolean grabado = false ;
//////////////////////////////// Horario  /////////////////////////

/////////////////////////////////HORARIO DE ENCENDIDO////////////////////////////

int h_c1 = 8;   int m_c1 = 0;

////////////////////////////////HORARIO EN EL QUE SE APAGAN LAS LUCES////////////////////////////////
// dentro de la instalación Indoor  h=Hora, m=Minutos, s=Segundos

/////////////////////////////////HORARIO 1////////////////////////////
//CICLO DE 12 HORAS DE LUZ
int h1_c1 = 20;   int m1_c1 = 0;

//////////////////////////////// Horario 2 /////////////////////////
//CICLO DE 18 HORAS DE LUZ
int h1_c2 = 2;   int m1_c2 = 0;

//////////////////////////////// Horario 3 /////////////////////////
//CICLO DE 16 HORAS DE LUZ
int h1_c3 = 0;   int m1_c3 = 0;

//////////////////////////////// Horario 4 /////////////////////////
//CICLO DE 14 HORAS DE LUZ
int h1_c4 = 22;   int m1_c4 = 0;
//VARIABLES PARA LA GUARDAR FECHA DE INICIO
int n_dia;
int n_mes;
int n_anno;
//VARIABLES PARA LA FECHA FINALIZACIÓN DE MODO 1
int n1_dia;
int n1_mes;
int n1_anno;
int n1_minute;
int n1_second ;
//VARIABLES PARA LA FECHA FINALIZACIÓN DE MODO 2
int n2_dia;
int n2_mes;
int n2_anno;
int n2_minute;
int n2_second ;
//VARIABLES PARA LA FECHA FINALIZACIÓN DE MODO 3
int n3_dia;
int n3_mes;
int n3_anno;
int n3_minute;
int n3_second ;
//VARIABLES PARA LA FECHA FINALIZACIÓN DE MODO 4
int n4_dia;
int n4_mes;
int n4_anno;
int n4_minute;
int n4_second ;

////////////////////////////////// Void Setup() ///////////
void setup () {
  Serial.begin(9600); // Establece la velocidad de datos del puerto serie a 9600
  ///////CONFIGURACIÓN DE PINES CÓMO SALIDA///////
  paso = 0;
  estado = 0;
  pinMode(ledM1, OUTPUT);
  pinMode(ledM2, OUTPUT);
  pinMode(ledM3, OUTPUT);
  pinMode(ledM4, OUTPUT);
  pinMode(LEDHUMEDAD, OUTPUT);
  pinMode(LEDCO2, OUTPUT);
  pinMode(LEDTEMPERATURA, OUTPUT);
  pinMode(LEDESTADO0, OUTPUT);
  pinMode(LEDINTENSIDAD, OUTPUT);
  pinMode(pinBomba, OUTPUT);
  pinMode(pinVentilador, OUTPUT);
  ///////CONFIGURACIÓN DE PINES CÓMO ENTRADA////////
  pinMode(pinDH, INPUT);
  /////BEGIN
  Wire.begin();
  RTC.begin();
  // INICIALIZAMOS EL DHT11
  dht.begin();
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("INICIANDO INDOOR...INICIANDO INDOOR");
  lcd.print("...");
  lcd.setCursor(0, 1);
  lcd.print("...FARM...FARM...FARM...FARM");
  lcd.print("...");
  for (int iBegin = 0; iBegin < 50; iBegin++) {
    lcd.scrollDisplayLeft();
    delay(300);
  }
  // following line sets the RTC to the date & time this sketch was compiled
  RTC.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  //RTC.adjust(DateTime(2022, 2, 1, 9, 7, 30));
  lcd.clear(); 
  delay(2000);// Borra el  LCD
}
////////////////RESET//////////////////////7
void(* resetFunc) (void) = 0;//FUNCION DE RESETEO PARA CUANDO SE PRESIONA EL BOTON 3 VOLVER AL INICIO
////////////////////////////////// Void loop() ///////////
void loop() {
  getHora();
  button = false;
  if (paso == 0) {
    alarmas(0);
    do {
      // paso = recuperarFecha();
      lcd.setCursor(0, 1);
      lcd.print ("PRESS BTN:");
      delay(500);
      int x = analogRead(A0);    //Lee el valor de los contactos para escoger el horario
      Serial.println(x);
      if (x < 60) {
        lcd.print ("Right ");
        delay(300);
        paso = 1;
        estado = 2;
        apagarAlarmas(0);
        cronograma(2);
        button  = true;
      }
      else if (x < 200) {
        lcd.print ("Up ");
        delay(300);
        estado = 5;
        activar_luces(estado);
        button  = true;
      }
      else if (x < 400) {
        lcd.print ("Down  ");
        delay(300);
        desactivar_luces();
      }
      else if (x < 600) {
        lcd.print ("Left");
        delay(300);
        paso = 1;
        estado = 1;
        apagarAlarmas(0);
        cronograma(estado);
        activar_luces(estado);
        button  = true;
      }
      else if (x < 800) {
        paso = 1;
        estado = 1;
        cronograma(1);
        apagarAlarmas(0);
        lcd.print ("Select:");
        delay(300);
        button  = true;

      }
    } while (button  == false);
  }
  else {
    delay(200);
    lcd.clear();
    if (paso == 1) {                      //Horario de Encendido de encendido de luz
      estado = dar_horario_encendido();
      paso=2;
    }
    else if (paso == 2) {                 //Verificar estado de las luces en las plantas
      float valorLDRCORNER1 = analogRead(A11);
      float valorLDRCORNER2 = analogRead(A12);
      float valorLDRCORNER3 = analogRead(A13);
      float valorLDRCORNER4 = analogRead(A14);
      float valorLDRCENTER = analogRead(A15);
      nivel_5_ldrs(valorLDRCORNER1, valorLDRCORNER2, valorLDRCORNER3, valorLDRCORNER4, valorLDRCENTER); //insertar función
      paso=3;
    }
    else if (paso == 3) {                  //Verificar niveles de CO2
      NivelCo2(MQ2_PIN);
      paso=4;
    }
    else if (paso ==4) {
      // Leemos la humedad relativa
      float h = dht.readHumidity();
      // Leemos la temperatura en grados centígrados (por defecto)
      float t = dht.readTemperature();
      // Comprobamos si ha habido algún error en la lectura e imprimimos valores de Temperatura,humedad y calor
      HumedadTemperatura(t, h);
      digitalWrite(pinVentilador, LOW);     // apaga los ventiladores
    }
    else if (paso == 5) {

      if (analogRead(Hsuelo) > 450) {
        lcd.setCursor(0, 1);           // Borra parte de la pantalla del LCD
        lcd.print("                  ");
        lcd.setCursor(0, 1);           // Borra parte de la pantalla del LCD
        lcd.print("BAJA HUMEDAD");
        delay(200);
        activar_bombariego();
      }
      else {
        lcd.setCursor(0, 1);           // Borra parte de la pantalla del LCD
        lcd.print("                  ");
        lcd.setCursor(0, 1);           // Borra parte de la pantalla del LCD           // Borra parte de la pantalla del LCD
        lcd.print("HUMEDAD OPTIMA");
        delay(200);
      }
      digitalWrite(pinBomba, LOW);          // apaga la bomba de riego
      
    }
    paso = 1;
    
    digitalWrite(pinVentilador, LOW);     // apaga los ventiladores
    digitalWrite(pinBomba, LOW);          // apaga la bomba de riego
    delay(500);                    // La informaci¢n se actualiza cada 1/2 seg.

  }
}
/////////////////////////////////////SUBRUTINAS/////////////////////////////////////
void leer_Buttons(int Estado) {
  lcd.setCursor(0, 1);
      lcd.print ("PRESS BTN:");
  delay(3000);
  int x = analogRead(A0);    //Lee el valor de los contactos para escoger el horario
  Serial.println(x);
  
  if (x < 60) {
    lcd.print ("Right ");
    estado = 2;
    cronograma(2);
  }
  else if (x < 200) {
    lcd.print ("Up    ");
    if (button  = true) {
      activar_luces(estado);
      button  = false;
      apagarAlarmas(2);
    }
    else {
      desactivar_luces();
    }
  }
  else if (x < 400) {
    lcd.print ("Down  ");
    estado = 3;
    cronograma(3);
  }
  else if (x < 600) {
    lcd.print ("Left");
    estado = 4;
    cronograma(4);;
  }
  else if (x < 800) {
    lcd.print ("Select:");
    delay(1000);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print ("INDOOR APAGADO");
    lcd.setCursor(0, 1);
    lcd.print ("REINICIANDO");
    ///////////Borra los datos de la memoria EEPROM asignado///////////
    for (int iReinicio = 0; iReinicio < 14; iReinicio++) {
      EEPROM.write(iReinicio, 0);
    }
    delay(5000);
    resetFunc();  //LLAMA AL RESET Y VA AL INICIO DE EL CICLO
  }
}
///////////////////////Imprime la hora actual////////
void getHora() {
  lcd.clear();
  DateTime now = RTC.now();          // Obtiene la fecha y hora del RTC
  lcd.setCursor(0, 0);
  lcd.print("*INDOOR FARM*");
  delay(500);
  lcd.setCursor(0, 1);
  lcd.print("Date:");
  lcd.print(now.year(), DEC);
  lcd.print("/");
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.day(), DEC);
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Time: ");
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  lcd.print(now.minute(), DEC);
  lcd.print(":");
  lcd.print(now.second(), DEC);
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.println(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
}
/////////////////////////////// Calcula el dia de la Semana //////////////////////////////////
void cronograma(int value) {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(5, 1);
  lcd.print("CRONOGRAMA");
  // Asigana las fechas futuras donde habrá el cambio de luces
  //value equivale a estado
  if ((value == 1) && (grabado == false) ) {
    // calculate a date which is 0 days, 0 hours, 0 minutes, 30 seconds into the future
    DateTime now = RTC.now(); //fecha y hora del
    DateTime future (now + TimeSpan(0, 0, 0, 30));
    n1_anno = future.year();
    n1_dia = future.day();
    n1_mes = future.month();
    n1_minute = future.minute();
    n1_second = future.second();
    Serial.println("CRONOGRAMA");
    Serial.print(value);
    Serial.println(n1_minute);
    Serial.print(":");
    Serial.print(n1_second);
    grabado = true;
    EEPROM.write(1, n1_anno);
    EEPROM.write(2, n1_mes);
    EEPROM.write(3, n1_dia);
    EEPROM.write(13, grabado);
  }
  else if ((value == 2) && (grabado == true)) {
    // calculate a date which is 10 days, 0 hours, 0 minutes, 0 seconds into the future
    // calculate a date which is 0 days, 0 hours, 0 minutes, 30 seconds into the future
    DateTime now = RTC.now(); //fecha y hora del
    DateTime future (now + TimeSpan(0, 0, 0, 30));
    n2_dia = future.day();
    n2_mes = future.month();
    n2_minute = future.minute();
    n2_second = future.second();
    Serial.println("CRONOGRAMA");
    Serial.print(value);
    Serial.println(n2_minute);
    Serial.print(":");
    Serial.print(n2_second);
    grabado = false;
    EEPROM.write(4, n2_anno);
    EEPROM.write(5, n2_mes);
    EEPROM.write(6, n2_dia);
    EEPROM.write(13, grabado);
    encender(value);

  }
  if ((value == 3) && (grabado == false)) {
    // calculate a date which is 0 days, 0 hours, 0 minutes, 30 seconds into the future
    DateTime now = RTC.now(); //fecha y hora del
    DateTime future (now + TimeSpan(0, 0, 0, 30));
    n3_anno = future.year();
    n3_dia = future.day();
    n3_mes = future.month();
    n3_minute = future.minute();
    n3_second = future.second();
    Serial.println("CRONOGRAMA");
    Serial.print(value);
    Serial.println(n3_minute);
    Serial.print(":");
    Serial.print(n3_second);
    grabado = true;
    EEPROM.write(7, n3_anno);
    EEPROM.write(9, n3_dia);
    EEPROM.write(8, n3_mes);
    EEPROM.write(13, grabado);
  }
  else if ((value == 4) && (grabado == true)) {
    // calculate a date which is 20 days, 0 hours, 0 minutes, 0 seconds into the future
    // calculate a date which is 0 days, 0 hours, 0 minutes, 30 seconds into the future
    DateTime now = RTC.now(); //fecha y hora del
    DateTime future (now + TimeSpan(0, 0, 0, 30));
    n4_anno = future.year();
    n4_dia = future.day();
    n4_mes = future.month();
    n4_minute = future.minute();
    n4_second = future.second();
    Serial.println("CRONOGRAMA");
    Serial.print(value);
    Serial.println(n4_minute);
    Serial.print(":");
    Serial.print(n4_second);
    EEPROM.write(10, n4_anno);
    EEPROM.write(12, n4_dia);
    EEPROM.write(11, n4_mes);
    EEPROM.write(13, grabado);
    
  }
  delay(1000);
}
//////////////////////ASIGNAR ESTADO SEGÚN LA FECHA//////////////////////////
void encender(int Estado) {
  getHora();
  DateTime now = RTC.now(); //fecha y hora del
  minuto = now.minute();
  segundo = now.second();
  if ( (minuto == n2_minute) && (segundo >= n2_second) )  activar_luces(Estado);
}
int dar_horario_encendido() {
  DateTime now = RTC.now(); //fecha y hora del RTC
  n_anno = now.year();
  n_dia = now.day();
  n_mes = now.month();
  minuto = now.minute();
  segundo = now.second();
  int CambioDeEstado;
  if ((n1_anno == n_anno) && (n1_dia == n_dia) && (n1_mes == n_mes) && (minuto >= n1_minute) && (segundo >= n1_second)) {
    CambioDeEstado == 2;
    cronograma(2);
    lcd.setCursor(0, 1);
    lcd.print("                ");
  }
  if ((n2_anno == n_anno) && (n2_dia == n_dia) && (n2_mes == n_mes) && (minuto >= n2_minute) && (segundo >= n2_second)) {
    CambioDeEstado == 3;
    cronograma(3);
  }
  if ((n3_anno == n_anno) && (n3_dia == n_dia) && (n3_mes == n_mes) && (minuto >= n3_minute) && (segundo >= n3_second)) {
    CambioDeEstado == 4;
    cronograma(4);
  }
  if ((n4_anno == n_anno) && (n4_dia == n_dia) && (n4_mes == n_mes) && (minuto >= n4_minute) && (segundo >= n4_second)) {
    CambioDeEstado == 5;
  }
  return CambioDeEstado;
}

/////////////////////Condicional del Horario de Encendido Después de Iniciado el programa  //////////////////////////////////
void horario_encendido(int Estado) {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(5, 1);
  lcd.print("LUCES");
  delay(2000);
  Serial.print("Horario ");
  Serial.print(Estado);
  if (Estado < 5) {
    if ((hora == h_c1) && (minuto == m_c1) )  activar_luces(Estado);
  }
}
////////////////////Condicional del Horario 1  //////////////////////////////////
void horario_Apagado() {
  if ((hora == h1_c1) && (minuto == m1_c1) )  desactivar_luces();
  if ((hora == h1_c2) && (minuto == m1_c2) )  desactivar_luces();
  if ((hora == h1_c3) && (minuto == m1_c3) )  desactivar_luces();
  if ((hora == h1_c4) && (minuto == m1_c4) )  desactivar_luces();
}
int recuperarFecha() {
  DateTime now = RTC.now(); //fecha y hora del RTC
  n_anno = now.year();
  n_dia = now.day();
  n_mes = now.month();
  if (grabado == false) {
    //VARIABLES PARA LA FECHA FINALIZACIÓN DE MODO 1
    int n1_dia  = EEPROM.read(3);
    int n1_mes  = EEPROM.read(2);
    int n1_anno = EEPROM.read(1);
    //VARIABLES PARA LA FECHA FINALIZACIÓN DE MODO 2
    int n2_dia  = EEPROM.read(6);
    int n2_mes  = EEPROM.read(5);
    int n2_anno = EEPROM.read(4);
    //VARIABLES PARA LA FECHA FINALIZACIÓN DE MODO 3
    int n3_dia  = EEPROM.read(9);
    int n3_mes  = EEPROM.read(8);
    int n3_anno = EEPROM.read(7);
    //VARIABLES PARA LA FECHA FINALIZACIÓN DE MODO 4
    int n4_dia  = EEPROM.read(12);
    int n4_mes  = EEPROM.read(11);
    int n4_anno = EEPROM.read(10);
    grabado = EEPROM.read(13);
    if ((n1_anno <= n_anno) && (n1_dia <= n_dia)) {
      estado = 2;
      paso = 1;
    }
    if ((n1_anno >= n_anno) && (n1_dia >= n_dia) && (n1_mes >= n_mes) && (n2_dia <= n_dia) && (n2_mes <= n_mes)) {
      estado = 2;
      paso = 1;
      cronograma(2);

    }
    else if ((n2_anno >= n_anno) && (n2_dia >= n_dia) && (n2_mes >= n_mes) && (n3_dia <= n_dia) && (n3_mes <= n_mes)) {
      estado = 3;
      paso = 1;
      grabado = false;
      cronograma(3);


    }
    else if ((n3_anno >= n_anno) && (n3_dia >= n_dia) && (n3_mes >= n_mes) && (n4_dia >= n_dia) && (n4_mes >= n_mes)) {
      estado = 4;
      paso = 1;
      grabado = true;
      cronograma(4);

    }
    else if ((n4_anno >= n_anno) && (n4_dia >= n_dia) && (n4_mes >= n_mes)) {
      estado = 5;
      paso = 1;
    }
  }
  return paso;
}
/////////////////////////////// Funcion que activa las luces//////////////////////////////////
void activar_luces(int Estado) {
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  if (Estado == 1) { //CUANDO SE ENCUENTRA EN EL ESTADO UNO SE ACTIVARAN LAS LUCES VERDE Y AZUL
    digitalWrite(ledM1, HIGH);
    digitalWrite(ledM2, LOW);
    digitalWrite(ledM3, LOW);
    digitalWrite(ledM4, LOW);
    lcd.print("MODO1 ON");
    Serial.print("MODO1 ON");
  }
  if (Estado == 2) { //CUANDO SE ENCUENTRA EN EL ESTADO UNO SE ACTIVARAN LAS LUCES, BLANCA Y AZUL
    digitalWrite(ledM1, HIGH);
    digitalWrite(ledM2, HIGH);
    digitalWrite(ledM3, LOW);
    digitalWrite(ledM4, LOW);
    lcd.print("MODO2 ON");
    Serial.print("MODO2 ON ");
  }
  if (Estado == 3) { //CUANDO SE ENCUENTRA EN EL ESTADO UNO SE ACTIVARAN LAS LUCES ROJA, BLANCA Y AZUL
    digitalWrite(ledM1, LOW);
    digitalWrite(ledM2, HIGH);
    digitalWrite(ledM3, HIGH);
    digitalWrite(ledM4, LOW);
    lcd.print("MODO3 ON");
    Serial.print("MODO3 ON");
  }
  if (Estado == 4) { //CUANDO SE ENCUENTRA EN EL ESTADO UNO SE ACTIVARAN LAS LUCES BLANCA Y LAS AZUL
    digitalWrite(ledM1, LOW);
    digitalWrite(ledM2, LOW);
    digitalWrite(ledM3, HIGH);
    digitalWrite(ledM4, HIGH);
    lcd.print("MODO4 ON");
    Serial.print("MODO4 ON");
  }
  if (Estado == 5) { //CUANDO SE ENCUENTRA EN EL ESTADO UNO SE ACTIVARAN LAS LUCES BLANCA Y LAS AZUL
    digitalWrite(ledM1, HIGH);
    digitalWrite(ledM2, LOW);
    digitalWrite(ledM3, LOW);
    digitalWrite(ledM4, HIGH);
    Serial.print("MODO5 ON");
  }
  lcd.print("ILUMINACION");
  Serial.println("ILUMINACION");
  delay(1000);
}
/////////////////////////////// Funcion que desactiva las luces//////////////////////////////////
void desactivar_luces() {
  digitalWrite(ledM1, LOW);
  digitalWrite(ledM2, LOW);
  digitalWrite(ledM3, LOW);
  digitalWrite(ledM4, LOW);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("LUCES OF ");
  Serial.println("LUCES APAGADAS");
  delay(1000);
}
//FUNCION QUE ACTIVA LOS VENTILADORES
void activar_ventilacion() {
  digitalWrite(pinVentilador, HIGH);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("VENTILACIÓN ON");
  Serial.println("CAMBIO DE AIRE");
  delay(tiempoVentilacion);
}

//FUNCION QUE ACTIVA LA BOMBA DE RIEGO
void activar_bombariego() {
  digitalWrite( pinBomba, HIGH);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("RIEGO ON");
  Serial.println("BOMBA RIEGO POR GOTEO");
  delay(tiempoRiego);
}

///////////////////////NIVEL DE LDRS //////////////////////////////////////////
void nivel_5_ldrs(float valor1, float valor2,  float valor3, float valor4, float valor ) {
  //LECTURA E IMPRESIÓN DE VALORES DE LA LUZ EN LOS LDR
  // con respecto a la luz percibida por el LDR
  // Devolver el valor leido a nuestro monitor serial en el IDE de Arduino
  // Encender los leds apropiados de acuerdo al valor de ADC
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 5);
  lcd.print("LUZ EN CORNER 1");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  if (valor1 < luzMinima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ BAJA ");
  }
  if (valor1 > luzMaxima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ EXCESIVA ");
    alarmas(2);
    //desactivar_luces();
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("LUZ OPTIMA");
  }
  lcd.print(valor1);
  // Esperar unos milisegundos antes de actualizar
  delay(2000);
  // Encender los leds apropiados de acuerdo al valor de ADC
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("LUZ EN CORNER 2");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  if (valor2 < luzMinima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ BAJA ");
    estado = 2;
    activar_luces(estado);
  }
  if (valor2 > luzMaxima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ EXCESIVA ");
    alarmas(2);
    //desactivar_luces();
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("LUZ OPTIMA ");
  }
  // Esperar unos milisegundos antes de actualizar
  lcd.print(valor2);
  delay(2000);
  // Devolver el valor leido a nuestro monitor serial en el IDE de Arduino
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  // Encender los leds apropiados de acuerdo al valor de ADC
  lcd.setCursor(0, 1);
  lcd.print("LUZ EN CORNER 3");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  if (valor3 < luzMinima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ BAJA ");
    estado = 3;
    activar_luces(estado);
  }
  if (valor3 > luzMaxima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ EXCESIVA ");
    alarmas(2);
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("LUZ OPTIMA ");
  }
  // Esperar unos milisegundos antes de actualizar
  lcd.print(valor3);
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("LUZ EN CORNER 4");
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  if (valor4 < luzMinima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ BAJA ");
  }
  if (valor4 > luzMaxima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ EXCESIVA ");
    alarmas(2);
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("LUZ OPTIMA ");
  }
  // Esperar unos milisegundos antes de actualizar
  lcd.print(valor4);
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  // Encender los leds apropiados de acuerdo al valor de ADC
  lcd.setCursor(0, 1);
  lcd.print("LUZ EN EL CENTRO");
  // Esperar unos milisegundos antes de actualizar
  delay(2000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  if (valor < luzMinima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ BAJA ");
  }
  if (valor > luzMaxima) {
    lcd.setCursor(0, 1);
    lcd.print("LUZ EXCESIVA ");
    alarmas(2);
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("LUZ OPTIMA ");
  }
  // Esperar unos milisegundos antes de actualizar
  lcd.print(valor);
  delay(2000);
}

void HumedadTemperatura(float tIn, float hIn) {
  // Comprobamos si ha habido algún error en la lectura
  if (isnan(hIn) || isnan(tIn)) {
    Serial.println("Error obteniendo los datos del sensor DHT11");
    lcd.setCursor(0, 1);
    lcd.print("                ");
    lcd.setCursor(1, 1);
    lcd.print("..DHT11 ERROR..");
    return;
  }
  // Calcular el índice de calor en grados centígrados
  float hic = dht.computeHeatIndex(tIn, hIn, false);
  if (hic >= 29) {
    activar_ventilacion();
    alarmas(4);
  }
  else {
    apagarAlarmas(4);
  }
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Humedad: ");
  lcd.print(hIn);
  Serial.print("Humedad: ");
  Serial.print(hIn);
  delay(3000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(" *C ");
  lcd.print("Temperatura: ");
  lcd.print(tIn);
  Serial.print("Temperatura: ");
  Serial.print(tIn);
  Serial.print("*C ");
  delay(3000);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Índice de calor: ");
  lcd.print(hic);
  lcd.print(" *C ");
  Serial.print("Índice de calor:");
  Serial.print(hic);
  delay(3000);
}

/////////////////////////////Funciones MQ2//////////////////////////////////////////////
//Calcular e Imprimir valores de CO2
float NivelCo2(int MQ_PIN) {
  float rs_med = readMQ(MQ_PIN);      // Obtener la Rs promedio
  float concentration = getConcentration(rs_med / R0); // Obtener la concentración
  if (concentration <= 300) {
    activar_bombariego();
  }
  // Mostrar el valor de la concentración por serial
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("NIVEL DE CO2: ");
  lcd.print(concentration/10);
  lcd.print(" *ppm ");
  delay(3000);
  if (concentration <= 450) {
    activar_ventilacion(); ////////ACTIVARÁ LA VENTILACIÓN PARA VALORES BAJOS DE CO2
    alarmas(3);
  }
  else {
    apagarAlarmas(3);
  }
  Serial.println("Concentración CO2: ");
  Serial.println(concentration);
}
// Obtener la resistencia promedio en N muestras
float readMQ(int mq_pin)
{
  float rs = 0;
  for (int i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += getMQResistance(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }
  return rs / READ_SAMPLE_TIMES;
}
// Obtener resistencia a partir de la lectura analogica
float getMQResistance(int raw_adc)
{
  return (((float)RL_VALUE / 1000.0 * (1023 - raw_adc) / raw_adc));
}
// Obtener concentracion 10^(coord + scope * log (rs/r0)
float getConcentration(float rs_ro_ratio)
{
  return pow(10, coord + scope * log(rs_ro_ratio));
}
/////////////////ALARMAS////////////////////////////
void alarmas(int alarma) {
  if (alarma == 0) {
    digitalWrite(LEDESTADO0, HIGH );  //ENCIENDE UNA ALARMA INDICANDO QUE EL SISTEMA ESTÁ ENCENDIDO PERO NO SE ENCUENTRA EN UN CICLO DE CULTIVO
    Serial.println("Alarma 4 Encendida");
  }
  if (alarma == 2) {
    digitalWrite(LEDINTENSIDAD, HIGH);//ENCIENDE UNA ALARMA INDICANDO QUE LA LUZ ES MUY INTENSA EN ALGUNO DE LOS SECTORES
    Serial.println("Alarma 5 Encendida");
  }
  if (alarma == 3) {
    digitalWrite(LEDCO2, HIGH);       //ENCIENDE UNA ALARMA INDICANDO QUE EL SISTEMA ESTÁ LEYENDO NIVELES ALTOS DE CO2
    Serial.println("Alarma 2 Encendida");
  }
  if (alarma == 4) {
    digitalWrite(LEDTEMPERATURA, HIGH); //ENCIENDE UNA ALARMA INDICANDO QUE EL SISTEMA ESTÁ LEYENDO NIVELES ALTAS TEMPERATURAS
    Serial.println("Alarma 3 Encendida");
  }
  if (alarma == 5) {
    digitalWrite(LEDHUMEDAD, HIGH);    //ENCIENDE UNA ALARMA INDICANDO QUE EL SISTEMA ESTÁ LEYENDO BAJOS NIVELES DE HUMEDAD EN EL SUSTRATO
    Serial.println("Alarma 1 Encendida");
  }
}
void apagarAlarmas(int alarma) {
  if (alarma == 0) {
    digitalWrite(LEDESTADO0, LOW );       //APAGA LA ALARMA
    Serial.println("Alarma 0 apagada");
  }
  if (alarma == 2) {
    digitalWrite(LEDINTENSIDAD, LOW);     //APAGA LA ALARMA
    Serial.println("Alarma 0 apagada");
  }
  if (alarma == 3) {
    digitalWrite(LEDCO2, LOW);            //APAGA LA ALARMA
    Serial.println("Alarma 0 apagada");
  }
  if (alarma == 4) {
    digitalWrite(LEDTEMPERATURA, LOW);    //APAGA LA ALARMA
    Serial.println("Alarma 0 apagada");
  }
  if (alarma == 5) {
    digitalWrite(LEDHUMEDAD, LOW);        //APAGA LA ALARMA
    Serial.println("Alarma 0 apagada");
  }
}
