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
#include <IRremote.hpp>

#define LECTURA_VALORES 0
#define DELAY_CONJUNTO 1

// Definiciones de pines
/*LCD*/
#define RS 8
#define EN 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13

/*Alarma*/
#define BUZZER 3

/*Motor DC*/
#define PIN_ONOFF 5
#define PIN_EN1A_ANTIHOR 4
#define PIN_EN2A_HOR 7

/*IR*/
#define IR_PIN 14
#define IR_UP 70
#define IR_DOWN 21
#define IR_LEFT 68
#define IR_RIGHT 67
#define IR_OK 64
#define IR_AJUSTES 71
#define GUARDAR_EN_EEPROM 69

/*Motor paso a paso*/
#define PASOS_POR_REVOLUCION 200
#define PIN_MOTOR_PASO_1 15
#define PIN_MOTOR_PASO_2 16
#define PIN_MOTOR_PASO_3 17
#define PIN_MOTOR_PASO_4 18
#define ID_MOTOR_PASO 3

#define IZQUIERDA 0
#define DERECHA 3

#define ARRIBA 3
#define ABAJO 0

#define ZONA_MUERTA 1

// Variables globales
/*Fecha y hora*/
struct dateTime
{
  int hora;
  int minuto;
  int segundo;
  int dia;
  int mes;
  int anio;
};
struct dateTime fechaHora = {23, 59, 59, 20, 12, 2024};

enum menu_e
{
  RELOJ,
  ALARMA,
  VENTILADOR,
  SENSOR_ULTRASONIDOS,
  AJUSTES
};

/*Valores Moverse Menu*/
int ejex = 0;
int ejey = 0;

/*Boton*/
bool boton = true;
bool botonAnterior = true;
bool seHaPulsadoBoton = false;
bool seHaConfirmado = false;

/*LCD*/
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

/*Ultrasonidos*/
const int trigPin = 2;
const int echoPin = 6;
float distancia = 0;
float distanciaMinima = 10;

/*Menu actual*/
int menuActual = RELOJ;
int menuAnterior = -1;
bool ajustesActivos = false;
int guardarMenuPrevio = -1;

/*Motor Paso a paso*/
int motorSteps[8][4] = {
    {HIGH, LOW, LOW, LOW},
    {HIGH, HIGH, LOW, LOW},
    {LOW, HIGH, LOW, LOW},
    {LOW, HIGH, HIGH, LOW},
    {LOW, LOW, HIGH, LOW},
    {LOW, LOW, HIGH, HIGH},
    {LOW, LOW, LOW, HIGH},
    {HIGH, LOW, LOW, HIGH}};
int velocidadMotor = 60;
bool modoFijo = false;
bool encendidoMotor = true;

/*Motor DC*/
int tiempoApagado = 1;
bool encendido = false;
int velocidad = 1;

// Cinco presets de alarma, cada uno con una hora y un minuto, y un booleano que indica si esta activada o no (a 0 todo inicialmente)
int presetAlarmas[3][3] = {
    // Hora, minuto, activada
    {0, 0, 0},
    {0, 0, 0},
    {0, 0, 0}};
int alarmaActual = 0;

void subRutinaReloj(bool activa);
void subRutinaAlarma(bool activa);
void subRutinaVentilador(bool activa);
void subRutinaSensorUltrasonidos(bool activa);
void subRutinaAjustes(bool activa);

// Funciones de lectura de los perifericos
int leerIR();
int leerUltrasonidos();
void comoSeComportaEjeY();

// Funciones de control del tiempo
int haPasadoUnSegundo();
int haPasadoTiempoDesdeUltimaInvocacion(unsigned long int tiempo, int id);

// Funcion que se encarga de pintar la hora en el LCD, y de pintar las alarmas activadas (y cambiar entre el menu de reloj, alarma, ventilador y sensor de ultrasonidos)
// LLama a las funciones de subRutinaReloj, subRutinaAlarma, subRutinaVentilador y subRutinaSensorUltrasonidos y las pinta en el LCD (si les envia un true)
// Si no, se ejecutan en segundo plano
void pintarLCD();

// Funcion para leer un valor del puerto serie
int leerPuertoSerie();

// Funcion para resetear la EEPROM
void resetearEEPROM();

// Funcion para leer la EEPROM
void leerEEPROM();
void miPropioDelay(unsigned long int tiempo);
int leerPuertoSerie();

void resetearEEPROM()
{
  for (int i = 0; i < 1024; i++)
  {
    EEPROM.write(i, 0);
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Inicializacion del mando infrarrojos
  IrReceiver.begin(IR_PIN);
  pinMode(IR_PIN, INPUT);

  // Inicializacion del motor DC
  pinMode(PIN_ONOFF, OUTPUT);
  pinMode(PIN_EN1A_ANTIHOR, OUTPUT);
  pinMode(PIN_EN2A_HOR, OUTPUT);
  digitalWrite(PIN_ONOFF, 0);
  // Inicialmente sentido horario
  digitalWrite(PIN_EN1A_ANTIHOR, 0);
  digitalWrite(PIN_EN2A_HOR, 1);

  // Inicializacion del sensor de ultrasonidos
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Inicializacion del buzzer
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  // Inicializacion motor paso a paso
  pinMode(PIN_MOTOR_PASO_1, OUTPUT);
  pinMode(PIN_MOTOR_PASO_2, OUTPUT);
  pinMode(PIN_MOTOR_PASO_3, OUTPUT);
  pinMode(PIN_MOTOR_PASO_4, OUTPUT);

  // Inicializacion del LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Proyecto final");
  lcd.setCursor(0, 1);
  lcd.print("Proyectos 1");

  Serial.print("Helloo!");

  leerEEPROM();

  delay(1000);
}

void loop()
{

  static int cambioSentido = 0;
  static bool sentido = true, sentidoAnterior = true;

  if (haPasadoTiempoDesdeUltimaInvocacion(200, LECTURA_VALORES))
  {
    comoSeComportaEjeY();
    leerIR();
  }

  // Leer el ultrasonidos
  if (haPasadoTiempoDesdeUltimaInvocacion(600, 2) && !haPasadoTiempoDesdeUltimaInvocacion(200, LECTURA_VALORES) && encendidoMotor)
  {
    distancia = leerUltrasonidos();
    Serial.print("Distancia: ");
    Serial.println(distancia);
  }

  if (haPasadoTiempoDesdeUltimaInvocacion(7, ID_MOTOR_PASO))
  {
    if (!modoFijo)
    {
      if (encendidoMotor || (!encendidoMotor && cambioSentido != 2000))
      {
        // Cambiar de sentido cada PASOS_POR_REVOLUCION / 2 pasos
        if (sentido != sentidoAnterior)
        {
          sentidoAnterior = sentido;
        }
        subRutinaMotorPasoAPaso(sentido ? IZQUIERDA : DERECHA);

        if (cambioSentido == 2000)
        {
          cambioSentido = 0;
          sentido = !sentido;
        }
        else
        {
          cambioSentido++;
        }
      }
    }
  }

  if (haPasadoTiempoDesdeUltimaInvocacion(500, DELAY_CONJUNTO))
  {
    pintarLCD();
    if (haPasadoUnSegundo())
    {
      fechaHora.segundo++;
    }
  }
}

void subRutinaMotorPasoAPaso(int direccion)
{
  static int step = 0;

  digitalWrite(PIN_MOTOR_PASO_1, motorSteps[step][0]);
  digitalWrite(PIN_MOTOR_PASO_2, motorSteps[step][1]);
  digitalWrite(PIN_MOTOR_PASO_3, motorSteps[step][2]);
  digitalWrite(PIN_MOTOR_PASO_4, motorSteps[step][3]);

  if (direccion == IZQUIERDA)
  {
    step = (step + 1) % 8;
  }
  else
  {
    if (step == 0)
    {
      step = 7;
    }
    else
    {
      step = (step - 1) % 8;
    }
  }
}

void subRutinaReloj(bool activa)
{
  char buffer[20]; // Buffer para dar formato a la hora y los minutos

  if (activa)
  {

    lcd.clear();

    sprintf(buffer, "%02d:%02d:%02d", fechaHora.hora, fechaHora.minuto, fechaHora.segundo);
    lcd.setCursor(0, 0);
    lcd.print(buffer);
    lcd.print(" (hora)");
    sprintf(buffer, "%02d/%02d/%04d", fechaHora.dia, fechaHora.mes, fechaHora.anio);
    lcd.setCursor(0, 1);
    lcd.print(buffer);
    lcd.print(" (dia)");
  }

  // Actualizar la hora y la fecha (a modo de calendario)
  if (fechaHora.segundo == 60)
  {
    fechaHora.segundo = 0;
    fechaHora.minuto++;
    if (fechaHora.minuto == 60)
    {
      fechaHora.minuto = 0;
      fechaHora.hora++;
      if (fechaHora.hora == 24)
      {
        fechaHora.hora = 0;
        fechaHora.dia++;
        int diasEnMes = 31; // Inicialmente asumimos que todos los meses tienen 31 días
        if (fechaHora.mes == 2)
        {
          // Febrero tiene 28 o 29 días dependiendo del año bisiesto
          if ((fechaHora.anio % 4 == 0 && fechaHora.anio % 100 != 0) || (fechaHora.anio % 400 == 0))
          {
            diasEnMes = 29;
          }
          else
          {
            diasEnMes = 28;
          }
        }
        else if (fechaHora.mes == 4 || fechaHora.mes == 6 || fechaHora.mes == 9 || fechaHora.mes == 11)
        {
          // Abril, junio, septiembre y noviembre tienen 30 días
          diasEnMes = 30;
        }
        if (fechaHora.dia > diasEnMes)
        {
          fechaHora.dia = 1;
          fechaHora.mes++;
          if (fechaHora.mes == 13)
          {
            fechaHora.mes = 1;
            fechaHora.anio++;
          }
        }
      }
    }
  }
}

// Funcion que devuelve 1 si ha pasado un segundo
int haPasadoUnSegundo()
{
  static unsigned long int tiempoUltimaInvocacion = 0;
  if (millis() - tiempoUltimaInvocacion > 1000)
  {
    tiempoUltimaInvocacion = millis();
    return 1;
  }
  return 0;
}

// Funcion que se encarga de la activacion de la alarma, de la lectura de la hora, de guardar las alarmas en la EEPROM
void subRutinaAlarma(bool activa)
{
  char buffer[10]; // Buffer para dar formato a la hora y los minutos
  if (activa)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarma ");
    lcd.print(alarmaActual + 1);
    lcd.print(": ");
    sprintf(buffer, "%02d:%02dh", presetAlarmas[alarmaActual][0], presetAlarmas[alarmaActual][1]);
    lcd.print(buffer);
    lcd.setCursor(0, 1);
    lcd.print("Activada: ");
    lcd.print(presetAlarmas[alarmaActual][2] ? "SI" : "NO");
  }

  // Comprobar si alguna alarma esta activada y comprobar si se ha llegado a la hora de la alarma
  for (int i = 0; i < 3; i++)
  {
    if (presetAlarmas[i][2] && fechaHora.hora == presetAlarmas[i][0] && fechaHora.minuto == presetAlarmas[i][1])
    {
      // Activar la alarma
      menuAnterior = -1;
      lcd.setCursor(0, 0);
      lcd.clear();
      lcd.print("ALARMA ACTIVADA");
      lcd.setCursor(0, 1);
      lcd.print("Hora: ");
      lcd.print(presetAlarmas[i][0]);
      lcd.print(":");
      lcd.print(presetAlarmas[i][1]);

      // Sonar el buzzer -> 3 pitidos cortos y 3 pitidos largos 2 veces
      for (int j = 0; j < 2; j++)
      {
        for (int k = 0; k < 3; k++)
        {
          tone(BUZZER, 1000, 100);
          miPropioDelay((unsigned long int)100);
        }
        for (int k = 0; k < 3; k++)
        {
          tone(BUZZER, 1000, 500);
          miPropioDelay((unsigned long int)500);
        }
      }

      // Si se pulsa OK se desactiva la alarma
      presetAlarmas[i][2] = 0;
      menuAnterior = ALARMA;

      // Añade el tiempo detenido al la hora actual
      fechaHora.segundo += (3 * 100 + 3 * 500) * 2 / 1000;
    }
  }
}

// Funcion que se encarga de la activacion y desactivacion del ventilador, y de la velocidad del ventilador
void subRutinaVentilador(bool activa)
{
  if (activa)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Ventilador: ");
    lcd.print(encendido ? "ON" : "OFF");
    lcd.setCursor(0, 1);
    lcd.print("Velocidad: ");
    lcd.print(velocidad);
    lcd.print("/5");
  }
}

// Funcion que se encarga de la lectura del sensor de ultrasonidos, de la activacion y desactivacion del sensor de ultrasonidos, y de la cuenta atras
void subRutinaSensorUltrasonidos(bool activa)
{
  static int cuentaAtrasDist = 0;
  if (activa)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Sens. de pres.: ");
    lcd.print("ON");
    lcd.setCursor(0, 1);
    lcd.print("Modo: ");
    // Giratorio o fijo
    if (encendidoMotor)
    {
      lcd.print(modoFijo ? "Fijo" : "Giratorio");
    }
    else
    {
      lcd.print("OFF");
    }
  }

  
  // Si detecta algo en el rango establecido, se desactiva la cuenta atras
  if (distancia <= distanciaMinima)
  {
    cuentaAtrasDist = -1;
    encendido = true;
  }
  

  // Si no detecta nada en el rango establecido, se activa la cuenta atras
  if (distancia > distanciaMinima)
  {
    cuentaAtrasDist = tiempoApagado;
    if (cuentaAtrasDist == 0)
    {
      encendido = false;
    }
    else if (haPasadoUnMinuto() && cuentaAtrasDist > 0)
    {
      cuentaAtrasDist--;
    }

  }

}

int haPasadoUnMinuto()
{
  static unsigned long int tiempoUltimaInvocacion = 0;
  if (millis() - tiempoUltimaInvocacion > 60000)
  {
    tiempoUltimaInvocacion = millis();
    return 1;
  }
  return 0;
}

// Funcion que se encarga de los ajustes del reloj, de la alarma, del ventilador (sentido de giro) y del sensor de ultrasonidos
void subRutinaAjustes(bool activa)
{
  static int posPuntero[2] = {14, 0};
  if (activa)
  {
    switch (guardarMenuPrevio)
    {
    case RELOJ:
      if (!seHaPulsadoBoton)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Cambiar hora");
        // LCD print Simbolo flecha
        lcd.setCursor(0, 1);
        lcd.print("Cambiar fecha");
        Serial.print(ejey);
        if (ejey > ZONA_MUERTA)
        {
          posPuntero[1] = 0;
          ejey = ZONA_MUERTA;
        }
        else if (ejey < ZONA_MUERTA)
        {
          posPuntero[1] = 1;
          ejey = ZONA_MUERTA;
        }

        lcd.setCursor(posPuntero[0], posPuntero[1]);
        lcd.print((char)127);
      }

      if (seHaPulsadoBoton && !posPuntero[1])
      {
        // Cambiar la hora
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Inroduce datos");
        lcd.setCursor(0, 1);
        lcd.print("en puerto serie");

        do
        {
          Serial.println("Introduce la hora: ");
          fechaHora.hora = leerPuertoSerie();
        } while (fechaHora.hora < 0 || fechaHora.hora > 23);

        do
        {
          Serial.println("Introduce el minuto: ");
          fechaHora.minuto = leerPuertoSerie();
        } while (fechaHora.minuto < 0 || fechaHora.minuto > 59);

        do
        {
          Serial.println("Introduce el segundo: ");
          fechaHora.segundo = leerPuertoSerie();
        } while (fechaHora.segundo < 0 || fechaHora.segundo > 59);

        seHaPulsadoBoton = false;
      }

      if (seHaPulsadoBoton && posPuntero[1])
      {
        // Cambiar la fecha
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Introduce datos");
        lcd.setCursor(0, 1);
        lcd.print("en puerto serie");

        do
        {
          Serial.println("Introduce el dia: ");
          fechaHora.dia = leerPuertoSerie();
        } while (fechaHora.dia < 0 || fechaHora.dia > 31);

        do
        {
          Serial.println("Introduce el mes: ");
          fechaHora.mes = leerPuertoSerie();
        } while (fechaHora.mes < 0 || fechaHora.mes > 12);

        do
        {
          Serial.println("Introduce el anio: ");
          fechaHora.anio = leerPuertoSerie();
        } while (fechaHora.anio < 0 || fechaHora.anio > 9999);

        seHaPulsadoBoton = false;
      }

      break;
    case ALARMA:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Cambiar alarma ");
      lcd.print(alarmaActual + 1);
      lcd.print("?");

      lcd.setCursor(0, 1);
      lcd.print("OK -> Cambiar");

      if (seHaPulsadoBoton)
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Introduce datos");
        lcd.setCursor(0, 1);
        lcd.print("en puerto serie");

        do
        {
          Serial.println("Introduce la hora: ");
          presetAlarmas[alarmaActual][0] = leerPuertoSerie();
        } while (presetAlarmas[alarmaActual][0] < 0 || presetAlarmas[alarmaActual][0] > 23);

        do
        {
          Serial.println("Introduce el minuto: ");
          presetAlarmas[alarmaActual][1] = leerPuertoSerie();
        } while (presetAlarmas[alarmaActual][1] < 0 || presetAlarmas[alarmaActual][1] > 59);

        seHaPulsadoBoton = false;
      }

      break;

    case VENTILADOR:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Autoapagado: ");
      lcd.setCursor(0, 1);
      lcd.print(tiempoApagado);
      lcd.print(" min");

      lcd.setCursor(15, 0);
      lcd.print((char)94);
      lcd.setCursor(15, 1);
      lcd.print((char)118);

      if (ejey == ARRIBA)
      {
        tiempoApagado++;
        ejey = ZONA_MUERTA;
      }

      if (ejey == ABAJO)
      {
        tiempoApagado--;
        ejey = ZONA_MUERTA;
        if (tiempoApagado < 1)
        {
          tiempoApagado = 1;
        }
      }

      // Al pulsar OK se sale de los ajustes
      if (seHaPulsadoBoton)
      {
        ajustesActivos = false;
        seHaPulsadoBoton = false;
        menuActual = guardarMenuPrevio;
      }

      break;
    case SENSOR_ULTRASONIDOS:
      // Ajustes del sensor de ultrasonidos (minima distancia en cm)
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Distancia minima: ");
      lcd.setCursor(0, 1);
      lcd.print(distanciaMinima);
      lcd.print(" cm");

      lcd.setCursor(15, 0);
      lcd.print((char)94);
      lcd.setCursor(15, 1);
      lcd.print((char)118);

      if (ejey == ARRIBA)
      {
        distanciaMinima++;
        ejey = ZONA_MUERTA;
        if (distanciaMinima > 999)
        {
          distanciaMinima = 100;
        }
      }

      if (ejey == ABAJO)
      {
        distanciaMinima--;
        ejey = ZONA_MUERTA;
        if (distanciaMinima < 10)
        {
          distanciaMinima = 10;
        }
      }

      // Al pulsar OK se sale de los ajustes
      if (seHaPulsadoBoton)
      {
        ajustesActivos = false;
        seHaPulsadoBoton = false;
        menuActual = guardarMenuPrevio;
      }

      break;  
    }



  }
}

// Funcion que se encarga de pintar la hora en el LCD, y de pintar las alarmas activadas (y cambiar entre el menu de reloj, alarma, ventilador y sensor de ultrasonidos)
// Menu de reloj: Hora, minuto, segundo
// Menu de alarma: Cinco alarmas, cada una con una hora y un minuto, y un booleano que indica si esta activada o no (a 0 todo inicialmente)
// Menu de ventilador: Activar o desactivar el ventilador, y cambiar la velocidad del ventilador (con el mando infrarrojos o con el joystick)
// Menu de sensor de ultrasonidos: Activar o desactivar el sensor de ultrasonidos.
void pintarLCD()
{
  switch (menuActual)
  {
  case RELOJ:
    if (menuAnterior != RELOJ)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reloj");
      miPropioDelay((unsigned long int)300);
      lcd.clear();
    }
    // Menu de reloj
    subRutinaReloj(true);
    subRutinaAlarma(false);
    subRutinaVentilador(false);
    subRutinaSensorUltrasonidos(false);
    break;
  case ALARMA:
    if (menuAnterior != ALARMA)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Alarma");
      miPropioDelay((unsigned long int)300);
      lcd.clear();
    }
    // Menu de alarma
    subRutinaReloj(false);
    subRutinaAlarma(true);
    subRutinaVentilador(false);
    subRutinaSensorUltrasonidos(false);
    break;
  case VENTILADOR:
    if (menuAnterior != VENTILADOR)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ventilador");
      miPropioDelay((unsigned long int)300);
      lcd.clear();
    }
    // Menu de ventilador
    subRutinaReloj(false);
    subRutinaAlarma(false);
    subRutinaVentilador(true);
    subRutinaSensorUltrasonidos(false);
    break;
  case SENSOR_ULTRASONIDOS:
    if (menuAnterior != SENSOR_ULTRASONIDOS)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Sensor ultrasonidos");
      miPropioDelay((unsigned long int)300);
      lcd.clear();
    }
    // Menu de sensor de ultrasonidos
    subRutinaReloj(false);
    subRutinaAlarma(false);
    subRutinaVentilador(false);
    subRutinaSensorUltrasonidos(true);
    break;
  case AJUSTES:
    if (menuAnterior != AJUSTES)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Ajustes");
      miPropioDelay((unsigned long int)300);
      lcd.clear();
    }
    // Menu de ajustes
    subRutinaAjustes(true);
    subRutinaReloj(false);
    subRutinaAlarma(false);
    subRutinaVentilador(false);
    subRutinaSensorUltrasonidos(false);
    break;
  }
  menuAnterior = menuActual;
}

void leerEEPROM()
{
  // Leer la hora y la alarma de la EEPROM
  int direccion = 0;
  fechaHora.hora = EEPROM.read(direccion);
  fechaHora.minuto = EEPROM.read(direccion + 1);
  fechaHora.segundo = EEPROM.read(direccion + 2);

  fechaHora.dia = EEPROM.read(direccion + 3);
  fechaHora.mes = EEPROM.read(direccion + 4);

  // Leer los pares de digitos del año
  fechaHora.anio = EEPROM.read(direccion + 5) * 100 + EEPROM.read(direccion + 6);

  // Leer las alarmas
  for (int i = 0; i < 3; i++)
  {
    presetAlarmas[i][0] = EEPROM.read(direccion + 7 + i * 3);
    presetAlarmas[i][1] = EEPROM.read(direccion + 8 + i * 3);
    presetAlarmas[i][2] = EEPROM.read(direccion + 9 + i * 3);
  }
}

int leerPuertoSerie()
{
  int valor = 0;
  while (Serial.available() == 0)
    ;
  valor = Serial.parseInt();

  while (Serial.available() > 0)
    Serial.read(); // Limpiar el buffer de entrada

  return valor;
}

void miPropioDelay(unsigned long int tiempo)
{
  unsigned long int tiempoInicio = millis();
  while (millis() - tiempoInicio < tiempo)
    ;
}

int haPasadoTiempoDesdeUltimaInvocacion(unsigned long int tiempo, int id)
{
  static unsigned long int tiempoUltimaInvocacion[4] = {0, 0, 0, 0}; // Array para almacenar el tiempo de la última invocación para cada id
  if (millis() - tiempoUltimaInvocacion[id] > tiempo)
  {
    tiempoUltimaInvocacion[id] = millis();
    return 1;
  }
  return 0;
}

int leerIR()
{
  // Leer el mando infrarrojos
  static int lectura = 0; // se mantine el valor de la última lectura y se inicializa apagado

  if (IrReceiver.decode() > 0)
  {                                             // Sólo se hace algo si hay entrada en el receptor IR
    lectura = IrReceiver.decodedIRData.command; // Se lee el comando, entendemos que cualquier mando nos puede manejar
    if (IrReceiver.decodedIRData.protocol != UNKNOWN)
    { // Si el protocolo es conocido
      Serial.print("El comando recibido en DEC es: ");
      Serial.println(lectura, DEC);

      if (lectura == IR_UP)
      {
        ejey = ARRIBA;
      }
      else if (lectura == IR_DOWN)
      {
        ejey = ABAJO;
      }
      else if (lectura == IR_LEFT)
      {
        if (!ajustesActivos)
        {
          if (menuActual == 0)
          {
            menuActual = 4;
          }
          else
          {
            menuActual = (menuActual - 1) % 4;
          }
        }
        else
        {
          ejex = IZQUIERDA;
        }
      }
      else if (lectura == IR_RIGHT)
      {
        if (!ajustesActivos)
        {
          menuActual = (menuActual + 1) % 4;
        }
        else
        {
          ejex = DERECHA;
        }
      }
      else if (lectura == IR_OK)
      {
        seHaPulsadoBoton = true;
      }
    }

    if (lectura == IR_AJUSTES)
    {
      ajustesActivos = !ajustesActivos;
      if (ajustesActivos)
      {
        guardarMenuPrevio = menuActual;
        menuActual = AJUSTES;
      }
      else
      {
        menuActual = guardarMenuPrevio;
      }
    }

    if (lectura == GUARDAR_EN_EEPROM)
    {
      int direccion = 0;
      EEPROM.write(direccion, fechaHora.hora);
      EEPROM.write(direccion + 1, fechaHora.minuto);
      EEPROM.write(direccion + 2, fechaHora.segundo);

      EEPROM.write(direccion + 3, fechaHora.dia);
      EEPROM.write(direccion + 4, fechaHora.mes);

      // Guarda los pares de digitos del año
      EEPROM.write(direccion + 5, fechaHora.anio / 100);
      EEPROM.write(direccion + 6, fechaHora.anio % 100);

      for (int i = 0; i < 3; i++)
      {
        EEPROM.write(direccion + 7 + i * 3, presetAlarmas[i][0]);
        EEPROM.write(direccion + 8 + i * 3, presetAlarmas[i][1]);
        EEPROM.write(direccion + 9 + i * 3, presetAlarmas[i][2]);
      }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Datos guardados");
      lcd.setCursor(0, 1);
      lcd.print("en la EEPROM");
      miPropioDelay((unsigned long int)1000);
      lcd.clear();
      // Añadimos al tiempo detenido el tiempo que ha tardado en guardar los datos
      fechaHora.segundo += 1;
    }

    IrReceiver.resume(); // Se reinicia el receptor IR para la siguiente lectura
  }

  return lectura;
}

int leerUltrasonidos()
{
  // Leer el sensor de ultrasonidos
  long tiempo_pulso = 0;
  float distanciaCm = 0;
  digitalWrite(trigPin, LOW); // 4 micro seg en LOW para estabilizar el nuevo pulso
  delayMicroseconds(4);
  digitalWrite(trigPin, HIGH); // pulso de disparo de 10 micro seg
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);                           // finalización del pulso de disparo
  tiempo_pulso = pulseIn(echoPin, HIGH);                // lectura del tiempo de duración del pulso en echo
  distanciaCm = (tiempo_pulso * 343.0 / 2.0) / 10000.0; // distancia en cm

  return distanciaCm;
}

// Funcion que se encarga del comportamiento del eje Y y de boton en cada menu
void comoSeComportaEjeY()
{
  if (!ajustesActivos)
  {
    switch (menuActual)
    {
    case RELOJ:

      break;
    case ALARMA:
      if (ejey < ZONA_MUERTA)
      {
        if (alarmaActual == 0)
        {
          alarmaActual = 2;
        }
        else
        {
          alarmaActual = (alarmaActual - 1) % 3;
        }
        ejey = ZONA_MUERTA;
      }
      else if (ejey > ZONA_MUERTA)
      {
        alarmaActual = (alarmaActual + 1) % 3;
        ejey = ZONA_MUERTA;
      }

      if (seHaPulsadoBoton)
      {
        presetAlarmas[alarmaActual][2] = !presetAlarmas[alarmaActual][2];
        seHaPulsadoBoton = false;
      }

      break;
    case VENTILADOR:
      if (ejey == ARRIBA)
      {
        if (velocidad == 1)
        {
          velocidad = 5;
        }
        else
        {
          velocidad = (velocidad - 1) % 5;
        }
        ejey = ZONA_MUERTA;
      }
      if (ejey == ABAJO)
      {
        velocidad = (velocidad + 1) % 5;
        ejey = ZONA_MUERTA;
      }
      if (seHaPulsadoBoton)
      {
        encendido = !encendido;
        seHaPulsadoBoton = false;
      }
      if (encendido)
      {
        // Encender el motor DC
        analogWrite(PIN_ONOFF, map(velocidad, 1, 5, 1, 255));
      }
      else
      {
        // Apagar el motor DC
        digitalWrite(PIN_ONOFF, 0);
      }

      break;
    case SENSOR_ULTRASONIDOS:
      if (ejey == ARRIBA)
      {
        modoFijo = !modoFijo;
        ejey = ZONA_MUERTA;
      }
      else if (ejey == ABAJO)
      {
        modoFijo = !modoFijo;
        ejey = ZONA_MUERTA;
      }
      if (seHaPulsadoBoton)
      {
        // Activar o desactivar el sensor de ultrasonidos
        seHaConfirmado = true;
        encendidoMotor = !encendidoMotor;
        seHaPulsadoBoton = false;
      }

      break;
    }
  }
}