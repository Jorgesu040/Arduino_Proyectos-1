#include <LiquidCrystal.h>
#include <DHT.h>


// Configuración de pines
const int rs = 12;
const int en = 13;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;

const int pinSensor = 9;
float t;

// Crear un objeto de la clase LiquidCrystal
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//Crea el objeto Temperatura
DHT temperatura (pinSensor, DHT11);

void setup() {
  // Inicializar el LCD con 16 columnas y 2 filas
  Serial.begin(9600);
  lcd.begin(16, 2);
  // Imprimir "Hola Mundo" en el LCD
  lcd.print("Hola Mundo ");
  lcd.setCursor(0,1);
 

  temperatura.begin();
}

void loop() {
  //
  t = temperatura.readTemperature();  

  lcd.setCursor(0,0);
  lcd.print("La temperatura");

  lcd.setCursor(0,1);
  lcd.print(t);

  delay(500);
}
