#include <Arduino.h>
#include <LiquidCrystal.h>
#include <IRremote.hpp>
#include <Servo.h>
#include <avr/sleep.h>


// Definiciones de pines
/*LCD*/
#define RS 8
#define EN 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13
#define BACKLIGHT 7

/*Presests*/
#define CINCO_MINUTOS 12
#define DIEZ_MINUTOS 24
#define QUINCE_MINUTOS 94
#define VEINTE_MINUTOS 8
#define VEINTICINCO_MINUTOS 28
#define TREINTA_MINUTOS 90
#define TREINTA_Y_CINCO_MINUTOS 66
#define CUARENTA_MINUTOS 82
#define CUARENTA_Y_CINCO_MINUTOS 74
#define MULTIPLOS_5 25

/*IR*/
#define IR_PIN 14

/*Servo*/
#define SERVO_PIN 6

// Variables globales
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
Servo motor;
int lectura = 0; // se mantine el valor de la última lectura y se inicializa apagado
bool luzTrasera = true;

void setup()
{

  // Inicializacion del puerto serie
  Serial.begin(9600);

  // Inicializacion del LCD
  lcd.begin(16, 2);
  lcd.print("Controlador Aire");
  lcd.setCursor(0, 1);
  lcd.print("Iniciando...");
  pinMode(BACKLIGHT, OUTPUT);
  digitalWrite(BACKLIGHT, luzTrasera);
  // Inicializacion del mando infrarrojos
  IrReceiver.begin(IR_PIN);
  pinMode(IR_PIN, INPUT);

  // Inicializacion del servo
  motor.attach(SERVO_PIN);
  motor.write(255);
  delay(1000);
  motor.detach();
  delay(1000);

  lcd.clear();
  lcd.print("Controlador Aire");
  lcd.setCursor(0, 1);
  lcd.print("Iniciado :)");
  delay(1000);
}

void loop()
{

  // put your main code here, to run repeatedly:
  if (haPasadoTiempoDesdeUltimaInvocacion(100, 0) && !motor.attached())
  {
    leerIR();
    lcd.clear();
    lcd.print("Acciones:");
    lcd.setCursor(0, 1);
    lcd.print("on/off|temp|luz");
  }
}

// Funcion que se encarga de comprobar si ha pasado un tiempo determinado desde la última invocación que se reutiliza con diferentes ids
int haPasadoTiempoDesdeUltimaInvocacion(unsigned long int tiempo, int id)
{
  static unsigned long int tiempoUltimaInvocacion[] = {0, 0, 0, 0}; // Array para almacenar el tiempo de la última invocación para cada id
  if (millis() - tiempoUltimaInvocacion[id] > tiempo)
  {
    tiempoUltimaInvocacion[id] = millis();
    return 1;
  }
  return 0;
}

int leerIR()
{
  if (IrReceiver.decode() > 0)
  {                                             // Sólo se hace algo si hay entrada en el receptor IR
    lectura = IrReceiver.decodedIRData.command; // Se lee el comando, entendemos que cualquier mando nos puede manejar
    // Si el protocolo es conocido
    Serial.print("El comando recibido en DEC es: ");
    Serial.println(lectura, DEC);
    manejarDispostivo(lectura);

    IrReceiver.resume(); // Se reinicia el receptor IR para la siguiente lectura
  }
  return lectura;
}

void manejarDispostivo(int comando)
{
  switch (comando)
  {
  case 69: // Encender y apagar
    lcd.clear();
    lcd.print("Accion:");
    lcd.setCursor(0, 1);
    lcd.print("ON/OFF - Aire AC");
    motor.attach(SERVO_PIN);
    motor.write(0);
    delay(2000);
    motor.write(255);
    delay(1000);
    motor.detach();
    break;
  case 71: // Poner temporizador
  case 13:
    lcd.clear();
    lcd.print("Accion:");
    lcd.setCursor(0, 1);
    lcd.print("Temp. de ON/OFF");
    delay(1000);
    introducirTiempo();
    break;
  case 22:
    lcd.clear();
    lcd.print("Accion:");
    lcd.setCursor(0, 1);
    lcd.print("ON/OFF - luz LCD");
    delay(1000);
    luzTrasera = !luzTrasera;
    digitalWrite(BACKLIGHT, luzTrasera);
    break;
  default:
    lcd.clear();
    lcd.print("Boton no");
    lcd.setCursor(0, 1);
    lcd.print("reconocido");
    break;
  }
}

void introducirTiempo()
{

  int arrayValores[] = {5, 10, 15, 20, 25, 30, 35, 40, 45};
  int arrayPresets[] = {CINCO_MINUTOS, DIEZ_MINUTOS, QUINCE_MINUTOS, VEINTE_MINUTOS, VEINTICINCO_MINUTOS, TREINTA_MINUTOS, TREINTA_Y_CINCO_MINUTOS, CUARENTA_MINUTOS, CUARENTA_Y_CINCO_MINUTOS};

  lcd.clear();
  lcd.print("Introduce tiempo");
  lcd.setCursor(0, 1);
  lcd.print("1-9 -> 5-45(min)");
  static int tiempo = -1;
  delay(500);

  while (tiempo == -1)
  {

    if (haPasadoTiempoDesdeUltimaInvocacion(200, 2))
    {
      int lecturaAux = 0;
      if (IrReceiver.decode() > 0)
      { // Sólo se hace algo si hay entrada en el receptor IR
        lecturaAux = IrReceiver.decodedIRData.command;
        Serial.print("El TIEMPO recibido en DEC es: ");
        Serial.println(lecturaAux, DEC);
        IrReceiver.resume(); // Se reinicia el receptor IR para la siguiente lectura
      }
      for (int i = 0; i < 9; i++)
      {
        if (lecturaAux == arrayPresets[i])
        {
          tiempo = arrayValores[i];
          break;
        }
      }

      if (lecturaAux == 25){
        tiempo = mandoB();
      }

      if (lecturaAux == 7)
      {
        tiempo = -1;
        lcd.clear();
        lcd.print("Temp. cancelado");
        lectura = 0;
        delay(500);
        lcd.clear();
        lcd.print("Volviendo al");
        lcd.setCursor(0, 1);
        lcd.print("menu principal");
        return;
      }
    }
  }

  while (tiempo >= 0)
  {
    // Si ha pasado un minuto
    if (haPasadoTiempoDesdeUltimaInvocacion(60000, 1))
    {
      tiempo--;
    }

    // Si ha pasado un segundo
    if (haPasadoTiempoDesdeUltimaInvocacion(1000, 3))
    {
      if (tiempo == 0)
      {
        motor.attach(SERVO_PIN);
        motor.write(0);
        delay(2000);
        motor.write(255);
        delay(1000);
        motor.detach();
        lcd.clear();
        lcd.print("Temp. finalizado");
        break;
      }
      else
      {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Temp. activado");
        lcd.setCursor(0, 1);
        lcd.print("T. rest.: ");
        lcd.print(tiempo);
        lcd.print("min");
      }
    }
  }
}

int mandoB(){
  // Calcula un multiplo de 5, por cada vez que se pulsa el boton (SEÑAL IR 25)
  int lecturaAux = 0;
  static int contador = 0;
  lcd.clear();
  lcd.print("Pulsa para");
  lcd.setCursor(0, 1);
  lcd.print("sumar 5 minutos");
  delay(500);

  // Lee hasta que se pulsa el boton de confirmar (SEÑAL IR 28) y envuleve el numero hasta 45 minutos
  // Una vez se llega a 45 minutos, se resetea el contador
  // Una vez se pulsa el de confirmar, se sale de la funcion y retorna el valor del contador

  while (lecturaAux != 28)
  {
    lecturaAux = 0;
    if (IrReceiver.decode() > 0)
    { // Sólo se hace algo si hay entrada en el receptor IR
      lecturaAux = IrReceiver.decodedIRData.command;
      Serial.print("El TIEMPO recibido en DEC es: ");
      Serial.println(lecturaAux, DEC);
      IrReceiver.resume(); // Se reinicia el receptor IR para la siguiente lectura
    }

    if (lecturaAux == 25)
    {
      contador += 5;
      if (contador > 45)
      {
        contador = 0;
      }
      lcd.clear();
      lcd.print("Temporizador de");
      lcd.setCursor(0, 1);
      lcd.print(contador);
      lcd.print(" minutos");
    }
    delay(500);
  }

  return contador;
  
}

