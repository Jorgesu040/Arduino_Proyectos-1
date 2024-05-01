#include <Arduino.h>
#include <EEPROM.h>

#define PINBOTON 5
#define PINTRANSISTOR 6

long int tiempoDesdePulsacion = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  DDRD = B01000000;
}

void loop() {
  // put your main code here, to run repeatedly:
  int estadoBoton = digitalRead(PINBOTON);

  Serial.println(estadoBoton);

  if (estadoBoton == HIGH) {
    tiempoDesdePulsacion = millis();
  }

  if (millis() - tiempoDesdePulsacion > EEPROM.read(10) * 1000){
    digitalWrite(PINTRANSISTOR, HIGH);
  } else {
    digitalWrite(PINTRANSISTOR, LOW);
  }

}
