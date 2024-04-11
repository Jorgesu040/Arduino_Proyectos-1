#include <Arduino.h>

// Definiciones de constantes y variables globales
#define POT_PIN A0 // Pin de lectura del potenciómetro
#define ENA_1 8  // Pin de habilitación 1A
#define ENA_2 9  // Pin de habilitación 2A
#define EN1_2 11   // Pin de control 1-2

int speed = 0;         // Variable de velocidad (0-255)
int potRead = 0;       // Variable de lectura del potenciómetro
bool turnRight = true; // Variable para decidir la dirección de giro

void setup()
{
  // Abrir el puerto serial
  Serial.begin(9600);

  // Configurar los modos de los pines
  pinMode(ENA_1, OUTPUT);
  pinMode(ENA_2, OUTPUT);
  pinMode(EN1_2, OUTPUT);
  pinMode(POT_PIN, INPUT);
}

void loop()
{
  // Leer el potenciómetro
  potRead = analogRead(POT_PIN);

  // Decidir la dirección de giro
  turnRight = potRead > 512;

  // Decidir la velocidad
  speed = map(potRead, 0, 1023, 0, 64);
  
  // Mostrar los valores en el puerto serial
  Serial.print("Potenciómetro: ");
  Serial.print(potRead);
  Serial.print(" - Velocidad: ");
  Serial.print(speed);
  Serial.print(" - Dirección: ");
  Serial.println(turnRight ? "Derecha" : "Izquierda");

  // Controlar los motores
  if (turnRight) {
    digitalWrite(EN1_2, speed);
    digitalWrite(ENA_1, HIGH);
    digitalWrite(ENA_2, LOW);
  } else {
    digitalWrite(EN1_2, speed);
    digitalWrite(ENA_1, LOW);
    digitalWrite(ENA_2, HIGH);
  }

  delay(100);
   
}