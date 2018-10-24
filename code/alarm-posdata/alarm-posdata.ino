/*******************************************************************
 *  Programa para el control de un sistema de alarma con Ard-nano, *
 *  para la Floreria POSDATA                                       *
 *  El control se lo realiza por un teclado KR602.                 *
 *  Tiene adicional un control de timbre accionado por un NV5      *
 *                      chatsID (numeros) autorizados              *
 *                                                                 *
 *    Author:   Pablo Cardozo                                      *
 *    Company:  Electric Eye                                       *
 *    Position: Frontend Developer                                 *
 *                                                                 *
 *******************************************************************/
/*                   PIN-IN/OUT-teclado                            *
*     Data 0(Green): D2                                            *
*     Data 1(White): D3                                            *
*     beep (Purple): D4                                            *
*     LED (Gray):    D5                                            *
********************************************************************
*                    PIN-IN-sensores                               *
*     Z1:       D7                                                 *
*     timbre:   D9                                                 *
********************************************************************
*                    PIN-OUT-actuadores                            *
*     sirena:    D13                                               *
********************************************************************/

// Libraries
#include <EEPROM.h>
#include <Wiegand.h>

// Inputs
// These are pins connected to the sensors.
#define sensor_z1         7
#define sensor_timbre     9
// Outputs
#define beep           4
#define led            5
#define sirena         13

// Keys enter an escape
#define ent 	13
#define esc  	27

// The object that handles the wiegand protocol.
WIEGAND wg;

// Global Variables
int value;
int fin = 127;
int cont = 0;
int timeAlarm;
int timeSirena;
int contAlarm;
int contSirena = 0;
int contBell = 0;
int aux;
int timeDelaySirena = 60;
int timeBellOn = 300;
int timeBellOff = 200;
int timeDelayBell = 10000;
int timeB;
int timeB_aux = 0;
long dataKey = 0;
unsigned long currentTime = 0;
unsigned long currentTimeBell;

bool flagProg = false;
bool flagAlarm = false;
bool flagSirena = false;
bool flagActive = false;
bool flagDelaySirena = false;
bool flagDesactiveSirena = false;
bool flagBell = true;
bool flagBellBeep = false;


void setup() {
  Serial.begin(9600);
  Serial.println("Initialize Wiegand...");
  // Inizialize Weigand
  wg.begin();

  // Initialize pins as inputs 
  pinMode(sensor_z1, INPUT_PULLUP);
  pinMode(sensor_timbre, INPUT_PULLUP);
  // Initialize outputs
  pinMode(beep, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(sirena, OUTPUT);
  // initial state
  digitalWrite(led, HIGH);  
  Off();
  
  // EEPROM.write(0,8);
  // EEPROM.write(1,8);
  // EEPROM.write(2,6);
  // EEPROM.write(3,6);
  // EEPROM.write(4,fin);

  // EEPROM.write(50,50);
  // EEPROM.write(51,fin);
  // EEPROM.write(60,3);
  // EEPROM.write(61,fin);

  timeAlarm = ReadMemory(50);
  timeSirena = ReadMemory(60);
}

void loop() {
  if(wg.available()) {
    value = ReadKeyboard();
    if(value == 0) cont++;
    else cont = 0;
    if(value == esc) Escape();
    if(value == ent) Enter();

    dataKey *= 10;
    dataKey += value;
  }

  if(((millis() - currentTime) > 1000)) {
    currentTime = millis();
    if(flagAlarm) DelayAlarm();
    if(flagDelaySirena) DelaySirena();
  }

  if(flagActive) ActiveAlarm();

  if(flagBell && !flagAlarm && !flagActive) ActiveBell();
}

void ActiveAlarm() {
  if(digitalRead(sensor_z1) == HIGH) {
    currentTime = millis();
    while(digitalRead(sensor_z1) == HIGH) {
      if((millis() - currentTime) > 600) {
        aux = led;
        digitalWrite(beep, LOW);
        delay(500);
        digitalWrite(beep, HIGH);
        flagActive = false;
        flagSirena = true;
        flagAlarm = true;
        break;
      }
    }
  }
}

void ActiveBell() {  
  if((millis() - currentTimeBell) > timeB_aux) {
    if(digitalRead(sensor_timbre) == HIGH) {
      timeB_aux = 0;
      flagBellBeep = true;    
    }
    if(flagBellBeep) {
      if(contBell == 4) timeB = 1500;
      if(contBell > 8) {
        digitalWrite(beep, HIGH);
        contBell = 0;
        flagBellBeep = false;
        timeB_aux = timeDelayBell;
        currentTimeBell = millis();
      }
      if((millis() - currentTimeBell) > timeB) {
        currentTimeBell = millis();
        contBell++;
        digitalWrite(beep, !digitalRead(beep));
        if(!digitalRead(beep)) timeB = timeBellOn;
        else timeB = timeBellOff;
      }
    }
  }
}

void DelayAlarm() {
  contAlarm++;
  digitalWrite(aux, !digitalRead(aux));
  if(contAlarm > timeAlarm) {
    digitalWrite(beep, HIGH);
    contAlarm = 0;
    flagAlarm = false;
    flagActive = true;
    Serial.println("Alarma Armada");
    if(flagDesactiveSirena) {
      flagActive = false;
      flagDesactiveSirena = false;
      flagDelaySirena = true;
      Off();
      flagSirena = false;
    }
    if(flagSirena) {
      flagActive = false;
      ActiveSirena();
    }
  }
}

void DelaySirena() {
  contAlarm++;
  if(contAlarm > timeDelaySirena) {
    contAlarm = 0;
    flagDelaySirena = false;
    ActiveSirena();
  }
}

void Escape() {
  flagProg = false;
  value = 0;
  dataKey = 0;
}

void Enter() {
  Serial.print("ent: ");
  Serial.println(dataKey);
  if(dataKey == ReadMemory(0)) {
    aux = beep;
    flagAlarm = true;
    timeAlarm = ReadMemory(50);
    contAlarm = 0;
    digitalWrite(led, LOW);
    if(flagSirena || flagActive) {
      flagAlarm = false;
      flagSirena = false;
      flagActive = false;
      flagDelaySirena = false;
      Off();
      Init();
      digitalWrite(led, HIGH);
    }
  }
  if(dataKey == 999) {
    flagBell = !flagBell;
  }
  value = 0;
  dataKey = 0;
}

void ActiveSirena() {
  digitalWrite(sirena, HIGH);          //Active alarm
  flagSirena = true;
  Serial.println("Sirena Activada");
  flagDesactiveSirena = true;
  flagAlarm = true;
  timeAlarm = timeSirena * 60;
}

int ReadKeyboard() {  
  Serial.print("Wiegand HEX = ");
  Serial.print(wg.getCode(),HEX);
  Serial.print(", Type W");
  Serial.println(wg.getWiegandType());

  return wg.getCode();
}

long ReadMemory ( int i) {
  long val = 0;
  int aux = i;
  while((EEPROM.read(i) != fin)) {
    val *= 10;
    val += EEPROM.read(i);
    i++;
    if(i>=(aux+10)){
        break;
    }
  }
  return val;
}

void Off() {
  digitalWrite(sirena, LOW);       
  digitalWrite(beep, HIGH);
  Serial.println("Sirena Apagada");
}

void Init() {
  flagProg = false;
  flagAlarm = false;
  flagSirena = false;
  flagActive = false;
  flagDelaySirena = false;
  flagDesactiveSirena = false;
}
