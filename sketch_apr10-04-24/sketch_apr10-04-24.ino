#include <Arduino.h>

#define pinPotenciometro A0

const int pinMotor1 = 6;
const int pinMotor2 = 7;
const int pinMotor3 = 8;
const int pinMotor4 = 9;

int velocidadMotor = 100;
int bobinados = 4;
int factor_reduccion = 64;
int numero_posiciones = 32;
int retardo_minimo = 10;
long int retardo = 0;

int Pasos_Vuelta = 20;

int anteriorPot = 0;

void setup()
{
  pinMode(pinMotor1, OUTPUT);
  pinMode(pinMotor2, OUTPUT);
  pinMode(pinMotor3, OUTPUT);
  pinMode(pinMotor4, OUTPUT);
  Serial.begin(9600);
  retardo = 10;
  if (retardo < retardo_minimo)
  {
    retardo = retardo_minimo;
  }
}

void loop()
{
  // Lee el valor del potenciómetro (0 a 1023)
  int valorPot = analogRead(pinPotenciometro);
  
  Serial.print("Valor del potenciómetro: ");
  Serial.print(valorPot);
  Serial.print("\t");

  // Decide la dirección en función del valor del potenciómetro
  if (valorPot < 512)
  {
    // Si el valor es menor que 512, gira en un sentido
    for (int i = 0; i < Pasos_Vuelta; i++)
    {
      sentido_Reloj(retardo);
    }

    if (anteriorPot > 512 && valorPot < 512)
    {
      Serial.println("Cambio de sentido");
    }
  }
  else if (valorPot > 512) // Add this condition to change direction when it goes over 512
  {
    // Si el valor es mayor que 512, gira en el otro sentido
    for (int i = 0; i < Pasos_Vuelta; i++)
    {
      sentido_antiReloj(retardo);
    }
    if (anteriorPot < 512 && valorPot > 512)
    {
      Serial.println("Cambio de sentido");
    }
  }

  anteriorPot = valorPot;
}

void sentido_Reloj(int retardo)
{
  static int ultimo_input = 0;
  int tablaPasos[8] = {B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001};
  ultimo_input++;

  if (ultimo_input == 8)
  {
    ultimo_input = 0;
  }

  digitalWrite(pinMotor1, bitRead(tablaPasos[ultimo_input], 0));
  digitalWrite(pinMotor2, bitRead(tablaPasos[ultimo_input], 1));
  digitalWrite(pinMotor3, bitRead(tablaPasos[ultimo_input], 2));
  digitalWrite(pinMotor4, bitRead(tablaPasos[ultimo_input], 3));
  delay(retardo);
}

void sentido_antiReloj(int retardo)
{
  static int ultimo_input = 0;
  int tablaPasos[8] = {B1000, B1100, B0100, B0110, B0010, B0011, B0001, B1001};
  ultimo_input--;

  if (ultimo_input < 0)
  {
    ultimo_input = 7;
  }

  digitalWrite(pinMotor1, bitRead(tablaPasos[ultimo_input], 0));
  digitalWrite(pinMotor2, bitRead(tablaPasos[ultimo_input], 1));
  digitalWrite(pinMotor3, bitRead(tablaPasos[ultimo_input], 2));
  digitalWrite(pinMotor4, bitRead(tablaPasos[ultimo_input], 3));
  delay(retardo);
}