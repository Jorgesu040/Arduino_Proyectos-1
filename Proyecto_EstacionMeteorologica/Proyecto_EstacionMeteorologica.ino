#include <DHT.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define RS 8
#define EN 9
#define D4 4
#define D5 5
#define D6 6
#define D7 7

#define SENTEMP 13
#define SENINCL 12
#define SENLUM A0

#define BUZZERPIN 2

#define BOTON 11

#define PINSERVO 10

// Numero de giros del toldo
#define TOLDO 3

#define ROJO A2
#define VERDE A4
#define AZUL A5

float temperatura;
float humedad;
int luminosidad;
bool viento;
int lecturaBoton = 0;
int lecturaAnterior = 0;
int casoActual = 0;
int scroll = 0;
unsigned long tiempoActual = 250;
unsigned long tiempoAnterior = 0;       // Almacena el tiempo de la última ejecución
unsigned long tiempoAnteriorViento = 0; // Almacena el tiempo de la última ejecución para saber cuanto tiempo hace que hay viento
int hayViento = 0;

unsigned long buttonPressTime = 0; // Almacena el tiempo de la última ejecución para saber cuanto tiempo se ha pulsado el boton

int valorReferenciaSoleado = 7; // Valor de referencia para un día soleado (0 a 10)

// Variables para el toldo
int rotacion1 = 0;
int rotacion2 = 0;
unsigned long tolodoAnterior = 0;

// Crear un objeto de la clase LiquidCrystal
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

// Crea el objeto Temperatura
DHT sensorTemperatura(SENTEMP, DHT11);

// Crea el objeto Servo
Servo servo;

void setup()
{
  // Inicializa el puerto serie
  Serial.begin(9600);

  servo.attach(PINSERVO);
  // Establece los pines del sensor DHT11 a input (por defecto pero para evitar errores)
  pinMode(SENTEMP, INPUT);
  pinMode(SENINCL, INPUT);
  pinMode(BOTON, INPUT);

  // Establece el LCD
  lcd.begin(16, 2);

  // Calcula la luminosidad
  luminosidad = map(analogRead(SENLUM), 100, 1000, 1, 10);

  mensajeDeBienvenida();

  sensorTemperatura.begin();

  delay(500);
}

void mensajeDeBienvenida()
{
  if (luminosidad > 3)
  {
    lcd.setCursor(0, 0);
    lcd.print("              ");
    lcd.setCursor(0, 0);
    lcd.print("Buenos dias!");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Buenas noches!");
  }

  lcd.setCursor(0, 1);
  lcd.print("Pulsa el boton para cambiar de modo");
}

void loop()
{
  leerSensores();
  imprimirSensores();
  // Espera a que se deje de pulsar el boton para volver a leer el boton
  if (tiempoActual - tiempoAnterior >= 250)
  {
    tiempoAnterior = tiempoActual; // Si han pasado 250 ms desde la última ejecución se actualiza el tiempo de la última ejecución
    lecturaBoton = digitalRead(BOTON);
    scroll++;
    if (scroll > 2 && casoActual == 0)
    {
      lcd.scrollDisplayLeft();
      scroll = 0;
      luzActual();
    }
    cambiarModos();
  }
}

// Añade una funcion que lea los sensores y los guarde en variables globales
void leerSensores()
{

  // Lee si hay viento
  if (viento == 1)
  {
    hayViento++;
    tiempoAnteriorViento = millis();
  }

  if (millis() - tiempoAnteriorViento >= 5000)
  {                // Si han pasado 5 segundos desde la última actualización
    hayViento = 0; // Reinicia el contador
  }

  // Guarda el tiempo en ejecución
  tiempoActual = millis();

  // Lee los valores de los sensores
  temperatura = sensorTemperatura.readTemperature();
  humedad = sensorTemperatura.readHumidity();
  luminosidad = map(analogRead(SENLUM), 100, 1000, 1, 10);
  viento = digitalRead(SENINCL);

  // Guarda el estado anterior del botón para saber si se ha pulsado o se mantine pulsado
  lecturaAnterior = lecturaBoton;
}

// Añade una funcion que imprima los valores de los sensores
void imprimirSensores()
{
  Serial.print("Lectura fotoresistor (1-10):\t");
  Serial.print(luminosidad);
  Serial.print("\tLectura temperatura:\t");
  Serial.print(temperatura);
  Serial.print("\tLectura humedad:\t");
  Serial.print(humedad);
  Serial.print("\tLectura inclinacion:\t");
  Serial.print(viento);
  Serial.print("\tLectura boton:\t");
  Serial.println(lecturaBoton);
}

// Funcion que basandose en la luminosidad actual, entiende si es de dia o de noche
void luzActual()
{
  if (luminosidad > 3)
  {
    lcd.setCursor(0, 0);
    lcd.print("              ");
    lcd.setCursor(0, 0);
    lcd.print("Buenos dias!");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Buenas noches!");
  }
}

// Añade una funcion que cambie de modo
void cambiarModos()
{
  if (lecturaBoton == LOW || lecturaAnterior == LOW)
  {
    buttonPressTime = millis();
  }
  if (millis() - buttonPressTime > 4000)
  { // Si el botón ha estado presionado por más de 4 segundos
    // Vuelve al modo 0
    volverInicio();
    casoActual = 0;
  }

  if (lecturaBoton == HIGH && lecturaAnterior == LOW)
  {
    casoActual++;
  }

  if (casoActual > 3)
  {
    casoActual = 1;
  }

  switch (casoActual)
  {
  case 1:
    temperaturaYhumedad();
    break;
  case 2:
    funcionViento();
    break;
  case 3:
    funcionNubosidad();
    break;
  }
}

// Añade una funcion que imprima la temperatura y la humedad
void temperaturaYhumedad()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tiempo: ");
  lcd.print(temperatura);
  lcd.print((char)223); // Imprime el símbolo de grados centígrados (º)
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Humedad: ");
  lcd.print(humedad);
  lcd.print("%");
}

// Añade una funcion que imprima si hay viento o no
void funcionViento()
{
  if (hayViento >= 1)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Actualmente: ");
    lcd.setCursor(0, 1);
    lcd.print("Hay viento");

    recogerToldo(1);
  }
  else if (hayViento == 0)
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Actualmente: ");
    lcd.setCursor(0, 1);
    lcd.print("No hay viento");

    recogerToldo(0);
  }
}
int ejecutarPitido = 0;
// Añade una funcion que recoga el toldo si hay viento
void recogerToldo(int recoger)
{
  // Si el ball switch está activo (se ha movido):
  if (recoger == 1)
  {
    // Hacer sonar el buzzer por 3 segundos:
    if (ejecutarPitido == 0)
    {
      tone(BUZZERPIN, 1000, 3000);
      ejecutarPitido = 1;
    }

    rotacion1 = 0;
    servo.write(0);
    if (millis() - tolodoAnterior > 400)
      servo.write(180);
    if (millis() - tolodoAnterior > 600)
      servo.write(0);
    if (millis() - tolodoAnterior > 800 && rotacion2 < TOLDO)
    {
      rotacion2++;
      servo.write(180);
      tolodoAnterior = millis();
    }
  }
  else
  {
    ejecutarPitido = 0;
    rotacion2 = 0;
    noTone(BUZZERPIN);
    // Si el tiempo transcurrido es mayor que el tiempo máximo sin movimiento:
    servo.write(180);
    if (millis() - tolodoAnterior > 400)
      servo.write(0);
    if (millis() - tolodoAnterior > 600)
      servo.write(180);
    if (millis() - tolodoAnterior > 800 && rotacion1 < TOLDO)
    {
      rotacion1++;
      servo.write(0);
      tolodoAnterior = millis();
    }
  }
}

// Añade una funcion que imprima el nivel de nubosidad actual
void funcionNubosidad()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Nubosidad actual: ");

  // Calcula el porcentaje de diferencia con el valor de referencia:
  int diferencia = 100 - map(luminosidad, 0, valorReferenciaSoleado, 0, 100);

  lcd.setCursor(0, 1);
  // Estima el nivel de nubosidad según el porcentaje de diferencia:
  // Muestra el nivel de nubosidad en el LCD:
  if (diferencia < 10)
  {
    lcd.print("Despejado");
  }
  else if (diferencia < 30)
  {
    lcd.print("Poco nuboso");
  }
  else if (diferencia < 50)
  {
    lcd.print("Nuboso");
  }
  else if (diferencia < 70)
  {
    lcd.print("Muy nuboso");
  }
  else
  {
    lcd.print("Cubierto");
  }
}

// Añade una funcion que vuelva al inicio del programa si se pulsa el boton durante unos segundos
void volverInicio()
{
  lcd.clear();
  if (luminosidad > 3)
  {
    lcd.setCursor(0, 0);
    lcd.print("              ");
    lcd.setCursor(0, 0);
    lcd.print("Buenos dias!");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("Buenas noches!");
  }

  lcd.setCursor(0, 1);
  lcd.print("Pulsa el boton para cambiar de modo");
}