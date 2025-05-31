#include <Arduino.h>
#include <LiquidCrystal.h>
#include <IRremote.hpp>
#include <Servo.h>
#include <LowPower.h>
#include <avr/power.h>

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
Servo motor;
int lectura = 0; // se mantine el valor de la última lectura y se inicializa apagado
bool luzTrasera = true;
bool firstTime = true;

volatile unsigned long lastInterruptTime = 0;
volatile bool irSignalDetected = false;

// Temporizador de inactividad
unsigned long lastActivityTime = millis();

class LcdManager {
public:
    LcdManager(byte rs, byte en, byte d4, byte d5, byte d6, byte d7, byte backlightPin)
        : _lcd(rs, en, d4, d5, d6, d7), _backlightPin(backlightPin), _internalBacklightState(true) {}

    void begin() {
        _lcd.begin(16, 2);
        pinMode(_backlightPin, OUTPUT);
        setBacklight(_internalBacklightState); // Inicializa el backlight con el estado interno
    }

    void print(const String& line1, const String& line2 = "") {
        _lcd.clear();
        _lcd.setCursor(0, 0);
        _lcd.print(line1);
        if (line2.length() > 0) {
            _lcd.setCursor(0, 1);
            _lcd.print(line2);
        }
    }

    void setBacklight(bool on) {
        _internalBacklightState = on;
        digitalWrite(_backlightPin, _internalBacklightState ? HIGH : LOW);
    }

    bool getBacklightState() {
        return _internalBacklightState;
    }
    
    void display() {
        _lcd.display();
    }

    void noDisplay() {
        _lcd.noDisplay();
    }

    void clear() {
        _lcd.clear();
    }

    void setCursor(byte col, byte row){
        _lcd.setCursor(col, row);
    }

    void printToLcd(const String& message) { 
        _lcd.print(message);
    }

    void printToLcd(int message) { 
        _lcd.print(message);
    }

private:
    LiquidCrystal _lcd;
    byte _backlightPin;
    bool _internalBacklightState; // Estado interno para el backlight
};

LcdManager myLcd(RS, EN, D4, D5, D6, D7, BACKLIGHT);

void powerSaveDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    LowPower.idle(SLEEP_15MS, ADC_OFF, TIMER2_OFF, TIMER1_OFF, TIMER0_ON, SPI_OFF, USART0_ON, TWI_OFF);
  }
}

// Delay normal para cuando esperamos activamente entrada IR
void activeDelay(unsigned long ms) {
  delay(ms);
}

void setup()
{

  // Deshabilitar periféricos innecesarios para ahorrar energía
  power_spi_disable();     // SPI no se usa
  power_twi_disable();     // I2C/TWI no se usa

  // Inicializacion de los pines
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(HARWARE_RESET, HIGH);
  pinMode(HARWARE_RESET, OUTPUT);

  // Inicializacion del puerto serie
  Serial.begin(9600);

  // Inicializacion del LCD
  myLcd.begin();
  myLcd.print("Controlador Aire", "Iniciando...");
  
  // Inicializacion del mando infrarrojos
  IrReceiver.begin(IR_PIN);
  pinMode(IR_PIN, INPUT);
  // Inicializacion del servo
  motor.attach(SERVO_PIN);
  motor.write(255);
  powerSaveDelay(1000);
  motor.detach();
  powerSaveDelay(1000);

  myLcd.print("Controlador Aire", "Iniciado :)");
  powerSaveDelay(1000);
}

void loop()
{

  if (firstTime)
  {
    myLcd.print("Acciones:", "on/off|temp|luz");
    firstTime = false;
  }

  // put your main code here, to run repeatedly:
  if (haPasadoTiempoDesdeUltimaInvocacion(100, 0) && !motor.attached())
  {
    leerIR(false);
  }

  if (millis() - lastActivityTime > INACTIVITY_THRESHOLD) {
    // Turn off components before sleeping
    myLcd.noDisplay();
    myLcd.setBacklight(false); // Usa el método de la clase
    
    // Turn off the built-in LED
    digitalWrite(LED_BUILTIN, LOW);

    // Attach the debounce ISR and enter sleep mode
    Serial.println("Entering sleep mode");
    Serial.flush();
    attachInterrupt(digitalPinToInterrupt(IR_PIN), irDetectedISR, FALLING);
    LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_ON);
    detachInterrupt(digitalPinToInterrupt(IR_PIN));
    
    // Attempt to decode the IR signal
    if (irSignalDetected) {      Serial.println("IR signal detected");      digitalWrite(LED_BUILTIN, HIGH);
      activeDelay(100); // Wait for the IR signal to be fully received
      if (IrReceiver.decode() > 0) {
        activeDelay(100); // Wait for the IR signal to be fully received
        // Check if the received command matches a valid command
        if (isValidCommand(IrReceiver.decodedIRData.protocol)) {
          // Valid IR command detected, resume operation
          myLcd.display();
          myLcd.setBacklight(luzTrasera); // Restaura el estado de luzTrasera
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
  powerSaveDelay(1000);
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
  {  case 69: // Encender y apagar
    myLcd.print("Accion:", "ON/OFF - Aire AC");
    motor.attach(SERVO_PIN);
    motor.write(0);
    powerSaveDelay(2000);
    motor.write(255);
    powerSaveDelay(1000);
    motor.detach();
    firstTime = true;
    break;
  case 71: // Poner temporizador
  case 13:
    myLcd.print("Accion:", "Temp. de ON/OFF");
    powerSaveDelay(1000);
    introducirTiempo();
    firstTime = true;
    break;
  case 22:
    myLcd.print("Accion:", "ON/OFF - luz LCD");
    powerSaveDelay(1000);
    luzTrasera = !luzTrasera;
    myLcd.setBacklight(luzTrasera); // Usa el método de la clase
    firstTime = true;
    break;
  case RESETARD:
    myLcd.print("Reiniciando");
    powerSaveDelay(1000);
    resetFunc();
    Serial.println("Arduino never run to this line");
    break;
  default:
    myLcd.print("Boton no", "reconocido");
    powerSaveDelay(1000);
    firstTime = true;
    break;
  }
}

void introducirTiempo()
{

  int arrayValores[] = {5, 10, 15, 20, 25, 30, 35, 40, 45};
  int arrayPresets[] = {CINCO_MINUTOS, DIEZ_MINUTOS, QUINCE_MINUTOS, VEINTE_MINUTOS, VEINTICINCO_MINUTOS, TREINTA_MINUTOS, TREINTA_Y_CINCO_MINUTOS, CUARENTA_MINUTOS, CUARENTA_Y_CINCO_MINUTOS};  myLcd.print("Introduce tiempo", "1-9 -> 5-45(min)");
  static int tiempo = -1;
  activeDelay(500);

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

      if (lecturaAux == 25) {
        tiempo = mandoB();
      }
      if (lecturaAux == 7) {
        tiempo = -1;
        myLcd.print("Temp. cancelado");
        lectura = 0;
        activeDelay(500);
        myLcd.print("Volviendo al", "menu principal");
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
      {        motor.attach(SERVO_PIN);
        motor.write(0);
        powerSaveDelay(2000);
        motor.write(255);
        powerSaveDelay(1000);
        motor.detach();
        myLcd.print("Temp. finalizado");
        break;
      }
      else
      {
        myLcd.clear();
        myLcd.setCursor(0, 0);
        myLcd.printToLcd("Temp. activado");
        myLcd.setCursor(0, 1);
        myLcd.printToLcd("T. rest.: ");
        myLcd.printToLcd(tiempo);
        myLcd.printToLcd("min");
      }

      if (haPasadoTiempoDesdeUltimaInvocacion(1000, 0) && !motor.attached())
      {
        // Escuchamos el IR para leer el comando de cancelar el temporizador
        if (leerIR(true) == 7) {
          tiempo = -1;
          myLcd.print("Temp. cancelado");
          lectura = 0;
          activeDelay(500);
          myLcd.print("Volviendo al", "menu principal");
          return;
        }

      }
    }
  }
}

int mandoB(){
  // Calcula un multiplo de 5, por cada vez que se pulsa el boton (SEÑAL IR 25)
  int lecturaAux = 0;  static int contador = 0;
  myLcd.print("Pulsa para", "sumar 5 minutos");
  activeDelay(500);

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
      myLcd.clear();
      myLcd.setCursor(0,0);
      myLcd.printToLcd("Temporizador de");
      myLcd.setCursor(0,1);      myLcd.printToLcd(contador);
      myLcd.printToLcd(" minutos");
    }
    activeDelay(500);
  }

  return contador;
  
}

