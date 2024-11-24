#include <Arduino.h>
#include <LiquidCrystal.h>
#include <IRremote.hpp>
#include <Servo.h>
#include <LowPower.h>


// Definiciones de pines
/*LCD*/
#define RS 8
#define EN 9
#define D4 10
#define D5 11
#define D6 12
#define D7 13
#define BACKLIGHT 7

#define HARWARE_RESET 4

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
#define IR_PIN 3
#define RESETARD 70

/*Servo*/
#define SERVO_PIN 6

/*Inactividad*/
#define INACTIVITY_THRESHOLD 10000

// Variables globales
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);
Servo motor;
int lectura = 0; // se mantine el valor de la última lectura y se inicializa apagado
bool luzTrasera = true;
bool firstTime = true;

volatile unsigned long lastInterruptTime = 0;
volatile bool irSignalDetected = false;

// Temporizador de inactividad
unsigned long lastActivityTime = millis();

void setup()
{

  // Inicializacion de los pines
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(HARWARE_RESET, HIGH);
  pinMode(HARWARE_RESET, OUTPUT);

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

  if (firstTime)
  {
    lcd.clear();
    lcd.print("Acciones:");
    lcd.setCursor(0, 1);
    lcd.print("on/off|temp|luz");
    firstTime = false;
  }

  // put your main code here, to run repeatedly:
  if (haPasadoTiempoDesdeUltimaInvocacion(100, 0) && !motor.attached())
  {
    leerIR(false);
    
  }

  if (millis() - lastActivityTime > INACTIVITY_THRESHOLD) {
    // Turn off components before sleeping
    lcd.noDisplay();
    digitalWrite(BACKLIGHT, LOW);
    
    // Turn off the built-in LED
    digitalWrite(LED_BUILTIN, LOW);

    // Attach the debounce ISR and enter sleep mode
    Serial.println("Entering sleep mode");
    Serial.flush();
    attachInterrupt(digitalPinToInterrupt(IR_PIN), irDetectedISR, FALLING);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);
    detachInterrupt(digitalPinToInterrupt(IR_PIN));
    
    
    // Attempt to decode the IR signal
    if (irSignalDetected) {
      Serial.println("IR signal detected");
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100); // Wait for the IR signal to be fully received
      if (IrReceiver.decode() > 0) {
        delay (100); // Wait for the IR signal to be fully received
        // Check if the received command matches a valid command
        if (isValidCommand(IrReceiver.decodedIRData.protocol)) {
          // Valid IR command detected, resume operation
          lcd.display();
          digitalWrite(BACKLIGHT, luzTrasera);
          lastActivityTime = millis(); // Reset inactivity timer
        } else {
          // Invalid IR command detected, go back to sleep
          irSignalDetected = false; // Reset the flag
        }
        IrReceiver.resume(); // Prepare for the next value
      } else {
        // No signal decoded, consider going back to sleep
      }
      irSignalDetected = false; // Reset the flag
    }
  }
}

void resetFunc()
{
  digitalWrite(HARWARE_RESET, LOW);
  delay(1000);
  digitalWrite(HARWARE_RESET, HIGH);

}

void irDetectedISR() {
  unsigned long interruptTime = millis();
  // Only accept the signal if it's been at least 100ms since the last one
  if (interruptTime - lastInterruptTime > 100) {
    irSignalDetected = true;
  }
  lastInterruptTime = interruptTime;
}

bool isValidCommand(int protocol) {
  if (protocol != UNKNOWN) {
    return true;
  } else {
    return false;
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

int leerIR(bool externo)
{
  if (IrReceiver.decode() > 0)
  {                                             // Sólo se hace algo si hay entrada en el receptor IR
    lectura = IrReceiver.decodedIRData.command; // Se lee el comando, entendemos que cualquier mando nos puede manejar
    // Si el protocolo es conocido
    Serial.print("El comando recibido en DEC es: ");
    Serial.println(lectura, DEC);
    if (!externo)
      manejarDispostivo(lectura);

    IrReceiver.resume(); // Se reinicia el receptor IR para la siguiente lectura

    if (IrReceiver.decodedIRData.protocol != UNKNOWN)
      lastActivityTime = millis(); // Restablece el temporizador de inactividad
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
    firstTime = true;
    break;
  case 71: // Poner temporizador
  case 13:
    lcd.clear();
    lcd.print("Accion:");
    lcd.setCursor(0, 1);
    lcd.print("Temp. de ON/OFF");
    delay(1000);
    introducirTiempo();
    firstTime = true;
    break;
  case 22:
    lcd.clear();
    lcd.print("Accion:");
    lcd.setCursor(0, 1);
    lcd.print("ON/OFF - luz LCD");
    delay(1000);
    luzTrasera = !luzTrasera;
    digitalWrite(BACKLIGHT, luzTrasera);
    firstTime = true;
    break;
  case RESETARD:
    lcd.clear();
    lcd.print("Reiniciando");
    delay(1000);
    resetFunc();
    Serial.println("Arduino never run to this line");
    break;
  default:
    lcd.clear();
    lcd.print("Boton no");
    lcd.setCursor(0, 1);
    lcd.print("reconocido");
    delay(1000);
    firstTime = true;
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

      if (haPasadoTiempoDesdeUltimaInvocacion(1000, 0) && !motor.attached())
      {
        // Escuchamos el IR para leer el comando de cancelar el temporizador
        if (leerIR(true) == 7) {
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

