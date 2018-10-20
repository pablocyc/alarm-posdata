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

// Libraries
#include <EEPROM.h>
#include <Wiegand.h>

// Inputs
// These are pins connected to the sensors.
#define sensor_z1         7
#define sensor_timbre     8
// Outputs
#define beep           4
#define led            6 
#define sirena         10

// The object that handles the wiegand protocol.
WIEGAND wg;


void setup() {
  Serial.begin(9600);
  Serial.println("Initialize Wiegand...");
  // Inizialize Weigand
  wg.begin();

  // Initialize pins as inputs 
  pinMode(sensor_z1, INPUT);
  pinMode(sensor_timbre, INPUT);
  // Initialize outputs
  pinMode(beep, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(sirena, OUTPUT);
  // initial state
  off();
  
}

void loop() {
	if(wg.available())
	{
		Serial.print("Wiegand HEX = ");
		Serial.print(wg.getCode(),HEX);
		Serial.print(", DECIMAL = ");
		Serial.print(wg.getCode());
		Serial.print(", Type W");
		Serial.println(wg.getWiegandType());    
	}
}





void off() {
    digitalWrite(sirena, HIGH);        //apagar sirena
    digitalWrite(led, HIGH);        //apagar led(rojo)
    digitalWrite(beep, HIGH);       //apagar speaker
}
