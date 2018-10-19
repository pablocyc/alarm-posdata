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
*     beep (Gray):   D4                                            *
*     LED (Purple):  D6                                            *
********************************************************************
*                    PIN-IN-sensores                               *
*     Z1:       D7                                                 *
*     timbre:   D8                                                 *
********************************************************************
*                    PIN-OUT-actuadores                            *
*     sirena:    D10                                               *
********************************************************************/

// Librerias
#include <EEPROM.h>
#include <Wiegand.h>

// Entradas
#define sensor-z1         7
#define sensor-timbre     8
// Salidas
#define beep           4
#define led            6 
#define sirena         10

// Configuracion Wiegand 
WIEGAND wg;


void setup() {
  Serial.begin(9600);
  wg.begin();
  // Configuracion Entradas
  pinMode(sensor-z1, INPUT);
  pinMode(sensor-timbre, INPUT);
  // Configuracion Salidas
  pinMode(beep, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(sirena, OUTPUT);
  // Estados iniciales
  off();
  
}

void loop() {
  if(wg.available()) {
    Serial.print("Wiegand HEX = ");
    Serial.print(wg.getCode(),HEX);        
    Serial.print(", Type W");       
    Serial.println(wg.getWiegandType());     

    int leer = wg.getCode();
  }
}




void off() {
    digitalWrite(sirena, HIGH);        //apagar sirena
    digitalWrite(led, HIGH);        //apagar led(rojo)
    digitalWrite(beep, HIGH);       //apagar speaker
}