#include <Arduino.h>

// Definiciones de constantes y variables globales
#define ENA_1 8  // Pin de habilitación 1A
#define ENA_2 9  // Pin de habilitación 2A
#define EN1_2 11   // Pin de control 1-2

int speed = 0;         // Variable de velocidad (0-255)
int potRead = 0;       // Variable de lectura del potenciómetro
bool turnRight = true; // Variable para decidir la dirección de giro

const int pinJoyX = A0;
const int pinJoyY = A1;
const int pinJoyButton = 9;
int Xvalue = 0;
int Yvalue = 0;
bool buttonValue = false;
bool OsnOff = false;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pinJoyButton, INPUT);
  pinMode(pinJoyX, INPUT);
  pinMode(pinJoyY, INPUT);
  pinMode(ENA_1, OUTPUT);
  pinMode(ENA_2, OUTPUT);
  pinMode(EN1_2, OUTPUT);
  pinMode(POT_PIN, INPUT);

  Serial.println("Modo de uso del Joystick: ejes X e Y y pulsador ");
  Serial.println("X: 0 a 1023 -> velocidad | Y: 0 a 1 -> Izq(0) o Derch(1) | Pulsador: 0 o 1");
  Serial.println("Pulsar el pulsador para encender o apagar el motor");
  Serial.println("Mover el eje Y para cambiar el sentido de giro");
  Serial.println("Mover el eje X para cambiar la velocidad");

  Serial.println("Pulsa el boton para empezar: ");
  while (digitalRead(pinJoyButton) == 0){
    delay(100);
  }

}

void loop() {
  // leer valores
  Xvalue = analogRead(pinJoyX);
  delay(1); // pausa entre lecturas analógicas recomendable
  Yvalue = analogRead(pinJoyY);
  buttonValue = digitalRead(pinJoyButton);
  // mostrar valores por serial
  Serial.print("X:");
  Serial.print(Xvalue);
  Serial.print(" | Y: ");
  Serial.print(Yvalue);
  Serial.print(" | Pulsador: ");
  Serial.println(buttonValue);
  delay(1000); // Una muestra por segundo

  if (buttonValue == 1){
    if (OsnOff == false){
      OsnOff = true;
      encender();
    }else{
      OsnOff = false;
      detener();
    }
  }

  if (OsnOff == true){
    comprobarSentido(Yvalue);
    manejarVelocidad(Xvalue);
  }

}

void detener(){
  Serial.println("Deteniendo");
  digitalWrite(ENA_1, LOW);
  digitalWrite(ENA_2, LOW);
  digitalWrite(EN1_2, LOW);
  speed = 0;

}

void encender(){
  Serial.println("Encendiendo");
  
}

void comprobarSentido(int valorY){

  potRead = analogRead(pinJoyY);

  // Decidir la dirección de giro
  turnRight = potRead > 512;

  if (turnRight) {
    digitalWrite(EN1_2, speed);
    digitalWrite(ENA_1, HIGH);
    digitalWrite(ENA_2, LOW);
  } else {
    digitalWrite(EN1_2, speed);
    digitalWrite(ENA_1, LOW);
    digitalWrite(ENA_2, HIGH);
  }

  Serial.println(turnRight ? "Derecha" : "Izquierda");
}


void manejarVelocidad(int valorX){
  
  // Decidir la velocidad
  speed = map(valorX, 0, 1023, 0, 64);
  
  // Mostrar los valores en el puerto serial
  Serial.print("Velocidad: ");
  Serial.println(speed);

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

