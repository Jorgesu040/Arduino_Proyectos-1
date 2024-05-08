
/*
Resumen:
Se trata de una alarma/reloj con un ventilador inteligente
•	Tiene función de reloj configurable por el puerto serie o un joystick
•	Tiene una función de alarma con buffer integrado que guarda las alarmas, aunque no haya corriente usando la memoria EEPROM
•	Tiene un ventilador controlable con un mando infrarrojos
•	Dicho ventilador se controla a través de un sensor de ultrasonidos, el cual gira sobre sí mismo (gracias a un motor paso a paso), 
  que activa una cuenta atrás cuando no detecte un objeto/usuario dentro de un rango personalizable.
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <IRremote.h>


#define IZQUIERDA 0
#define DERECHA 2

#define ARRIBA 2
#define ABAJO 0

#define ZONA_MUERTA 1
// Variables globales
/*Fecha y hora*/
struct dateTime {
  int hora;
  int minuto;
  int segundo;
  int dia;
  int mes;
  int anio;
} fechaHora;

enum menu_e {
  RELOJ,
  ALARMA,
  VENTILADOR,
  SENSOR_ULTRASONIDOS,
  AJUSTES
} menuActual;

/*Joystick*/
int pinX = A0;
int pinY = A1;
int ejex = 0;
int ejey = 0;

/*Boton*/
bool boton = false;
bool botonAnterior = false;

/*LCD*/
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);

/*Mando infrarrojos*/
int pinIR = 13;

/*Menu actual*/
menu_e menuActual = RELOJ;


// Cinco presets de alarma, cada uno con una hora y un minuto, y un booleano que indica si esta activada o no (a 0 todo inicialmente)
int presetAlarmas[5][3] = {
  // Hora, minuto, activada
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};



void subRutinaReloj(bool activa);
void subRutinaAlarma(bool activa);
void subRutinaVentilador(bool activa);
void subRutinaSensorUltrasonidos(bool activa);
void subRutinaAjustes(bool activa);


// Funciones de lectura de los perifericos
void leerJoystickXeY();
int leerBoton();
int leerIR();

// Funcion que se encarga de pintar la hora en el LCD, y de pintar las alarmas activadas (y cambiar entre el menu de reloj, alarma, ventilador y sensor de ultrasonidos)
// LLama a las funciones de subRutinaReloj, subRutinaAlarma, subRutinaVentilador y subRutinaSensorUltrasonidos y las pinta en el LCD (si les envia un true)
// Si no se ejecutan en segundo plano
void pintarLCD();


void leerEEPROM();
void miPropioDelay(long int tiempo);
int leerPuertoSerie();


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Inicializacion del LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Proyecto final");
  lcd.setCursor(0, 1);
  lcd.print("Proyectos 1");

  leerEEPROM();

  // Primera vez que se ejecuta el programa
  if (fechaHora.hora == 0 && fechaHora.minuto == 0 && fechaHora.segundo == 0) {
    do {
      Serial.println("Introduce la hora: ");
      fechaHora.hora = leerPuertoSerie();
    } while (fechaHora.hora < 0 || fechaHora.hora > 23);
    
    do {
      Serial.println("Introduce el minuto: ");
      fechaHora.minuto = leerPuertoSerie();
    } while (fechaHora.minuto < 0 || fechaHora.minuto > 59);

    do {
      Serial.println("Introduce el segundo: ");
      fechaHora.segundo = leerPuertoSerie();
    } while (fechaHora.segundo < 0 || fechaHora.segundo > 59);

    do {
      Serial.println("Introduce el dia: ");
      fechaHora.dia = leerPuertoSerie();
    } while (fechaHora.dia < 0 || fechaHora.dia > 31);

    do {
      Serial.println("Introduce el mes: ");
      fechaHora.mes = leerPuertoSerie();
    } while (fechaHora.mes < 0 || fechaHora.mes > 12);

    do {
      Serial.println("Introduce el anio: ");
      fechaHora.anio = leerPuertoSerie();
    } while (fechaHora.anio < 0);


  }
  
  delay(1000);

}




void loop() {
  
  // put your main code here, to run repeatedly:
  menuActual = leerBoton();
  leerJoystickXeY();
  leerIR();
  pintarLCD();
  
  miPropioDelay(1000);

}

void subRutinaReloj(bool activa) {
  if (activa) {
    lcd.setCursor(0, 0);
    lcd.print(fechaHora.hora);
    lcd.print(":");
    lcd.print(fechaHora.minuto);
    lcd.print(":");
    lcd.print(fechaHora.segundo);

    lcd.setCursor(0, 1);
    lcd.print(fechaHora.dia);
    lcd.print("/");
    lcd.print(fechaHora.mes);
    lcd.print("/");
    lcd.print(fechaHora.anio);
  }

  // Actualizar la hora y la fecha
  fechaHora.segundo++;
  if (fechaHora.segundo == 60) {
    fechaHora.segundo = 0;
    fechaHora.minuto++;
    if (fechaHora.minuto == 60) {
      fechaHora.minuto = 0;
      fechaHora.hora++;
      if (fechaHora.hora == 24) {
        fechaHora.hora = 0;
        fechaHora.dia++;
        if (fechaHora.dia == 32) {
          fechaHora.dia = 1;
          fechaHora.mes++;
          if (fechaHora.mes == 13) {
            fechaHora.mes = 1;
            fechaHora.anio++;
          }
        }
      }
    }
  }

  // Guardar la hora y la fecha en la EEPROM
  int direccion = 0;

  EEPROM.write(direccion, fechaHora.hora);
  EEPROM.write(direccion + 1, fechaHora.minuto);
  EEPROM.write(direccion + 2, fechaHora.segundo);

  EEPROM.write(direccion + 3, fechaHora.dia);
  EEPROM.write(direccion + 4, fechaHora.mes);
  EEPROM.write(direccion + 5, fechaHora.anio);


}

// Funcion que se encarga de la activacion de la alarma, de la lectura de la hora, de guardar las alarmas en la EEPROM
void subRutinaAlarma(bool activa) {
  if (activa) {
    for (int i = 0; i < 5; i++) {
      lcd.setCursor(i, 0);
      lcd.print(presetAlarmas[i][0]);
      lcd.print(":");
      lcd.print(presetAlarmas[i][1]);
      lcd.print(" ");
      lcd.setCursor(i, 1);
      lcd.print(presetAlarmas[i][2] ? "ON" : "OFF");
    }
  }

  // Comprobar si alguna alarma esta activada y comprobar si se ha llegado a la hora de la alarma
  for (int i = 0; i < 5; i++) {
    if (presetAlarmas[i][2] && fechaHora.hora == presetAlarmas[i][0] && fechaHora.minuto == presetAlarmas[i][1]) {
      // Activar la alarma
      lcd.setCursor(0, 0);
      lcd.print("ALARMA ACTIVADA");
    }
  }
}


// Funcion que se encarga de la activacion y desactivacion del ventilador (de forma manual o automatica)
void subRutinaVentilador(bool activa) {
  if (activa) {
    lcd.setCursor(0, 0);
    lcd.print("Ventilador: ");
    lcd.print("ON");
  }
}


// Funcion que se encarga de la lectura del sensor de ultrasonidos, de la activacion y desactivacion del sensor de ultrasonidos, y de la cuenta atras
void subRutinaSensorUltrasonidos(bool activa) {
  if (activa) {
    lcd.setCursor(0, 0);
    lcd.print("Sensor de presencia: ");
    lcd.print("ON");
    lcd.setCursor(0, 1);
    lcd.print("Distancia: ");
  }

  // Leer el sensor de ultrasonidos
  

}

// Funcion que se encarga de los ajustes del reloj, de la alarma, del ventilador (sentido de giro) y del sensor de ultrasonidos
void subRutinaAjustes(bool activa) {
  if (activa) {
    lcd.setCursor(0, 0);
    lcd.print("Ajustes");
    
    
  }
}



// Funcion que se encarga de pintar la hora en el LCD, y de pintar las alarmas activadas (y cambiar entre el menu de reloj, alarma, ventilador y sensor de ultrasonidos)
// Menu de reloj: Hora, minuto, segundo (y la posibilidad de cambiarlo con el joystick)
// Menu de alarma: Cinco alarmas, cada una con una hora y un minuto, y un booleano que indica si esta activada o no (a 0 todo inicialmente)
// Menu de ventilador: Activar o desactivar el ventilador, y cambiar la velocidad del ventilador (con el mando infrarrojos)
// Menu de sensor de ultrasonidos: Activar o desactivar el sensor de ultrasonidos, y cambiar el rango de deteccion (con el joystick) (asi como ajustar el tiempo de la cuenta atras) 
void pintarLCD(){
  switch (menuActual) {
    case RELOJ:
      // Menu de reloj
      subRutinaReloj(true);
      subRutinaAlarma(false);
      subRutinaVentilador(false);
      subRutinaSensorUltrasonidos(false);
      break;
    case ALARMA:
      // Menu de alarma
      subRutinaReloj(false);
      subRutinaAlarma(true);
      subRutinaVentilador(false);
      subRutinaSensorUltrasonidos(false);
      break;
    case VENTILADOR:
      // Menu de ventilador
      subRutinaReloj(false);
      subRutinaAlarma(false);
      subRutinaVentilador(true);
      subRutinaSensorUltrasonidos(false);
      break;
    case SENSOR_ULTRASONIDOS:
      // Menu de sensor de ultrasonidos
      subRutinaReloj(false);
      subRutinaAlarma(false);
      subRutinaVentilador(false);
      subRutinaSensorUltrasonidos(true);
      break;
    case AJUSTES:
      // Menu de ajustes
      subRutinaAjustes(true);
      subRutinaReloj(false);
      subRutinaAlarma(false);
      subRutinaVentilador(false);
      subRutinaSensorUltrasonidos(false);
      break;
  }


}


void leerEEPROM() {
  // Leer la hora y la alarma de la EEPROM
  int direccion = 0;
  fechaHora.hora = EEPROM.read(direccion);
  fechaHora.minuto = EEPROM.read(direccion + 1);
  fechaHora.segundo = EEPROM.read(direccion + 2);

  fechaHora.dia = EEPROM.read(direccion + 3);
  fechaHora.mes = EEPROM.read(direccion + 4);
  fechaHora.anio = EEPROM.read(direccion + 5);

  for (int i = 0; i < 5; i++) {
    presetAlarmas[i][0] = EEPROM.read(direccion + 6 + i * 3);
    presetAlarmas[i][1] = EEPROM.read(direccion + 7 + i * 3);
    presetAlarmas[i][2] = EEPROM.read(direccion + 8 + i * 3);
  }

}


int leerPuertoSerie() {
  int valor = 0;
  while (Serial.available() == 0);
    valor = Serial.parseInt();

    while (Serial.available() > 0) 
      Serial.read(); // Limpiar el buffer de entrada
    
    Serial.println(valor);

  return valor;
}

void miPropioDelay(long int tiempo) {
  long int tiempoInicio = millis();
  while (millis() - tiempoInicio < tiempo);
}

void leerJoystickXeY() {

  int xValue = analogRead(pinX);
  int yValue = analogRead(pinY);

  // Apply deadzone to x-axis
  if (xValue < 512 + 50 && xValue > 512 - 50) {
    xValue = 512;
  }

  // Apply deadzone to y-axis
  if (yValue < 512 + 50 && yValue > 512 - 50) {
    yValue = 512;
  }

  ejex = map(xValue, 0, 1023, IZQUIERDA, DERECHA);
  ejey = map(yValue, 0, 1023, ABAJO, ARRIBA);
}

int leerBoton() {
  boton = digitalRead(2);
  if (boton && !botonAnterior) {
    // Cambiar el menu
    menuActual = (menuActual + 1) % 5;
  }
  botonAnterior = boton;
  return boton;
}
  
