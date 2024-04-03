#include <Arduino.h>
#include <LiquidCrystal.h>
const int TriggerPin = 2;
const int EchoPin = 3;
const int rs = 12, en = 13, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("Bienvenido");
  delay(1000);
  lcd.clear();
}

void loop()
{
  float distancia = meas_dist(TriggerPin, EchoPin);
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 0);
  lcd.print("Distancia: ");
  lcd.print(distancia);
}

float meas_dist(int TriggerPin, int EchoPin)
{
  long tiempo_pulso = 0; 
  float distanciaCm = 0; 
  digitalWrite(TriggerPin, LOW); // 4 micro seg en LOW para estabilizar el nuevo pulso
  delayMicroseconds(4);
  digitalWrite(TriggerPin, HIGH); // pulso de disparo de 10 micro seg
  delayMicroseconds(10);
  digitalWrite(TriggerPin, LOW);         // finalización del pulso de disparo
  tiempo_pulso = pulseIn(EchoPin, HIGH); // lectura del tiempo de duración del pulso en echo
  distanciaCm = (tiempo_pulso * 343.0 / 2.0) / 10000.0; // distancia en cm
  return distanciaCm;
}

