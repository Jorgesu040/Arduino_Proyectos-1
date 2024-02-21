#include <Servo.h> 

Servo primerServo;

#define pinServo 9

#define pinAnalogico A0

int lectura; //Valor entre 0 y 1023
int anguloGirado;
float tension; //Valor entre 0 y 5



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  primerServo.attach(pinServo);
  pinMode(pinAnalogico, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  lectura = analogRead(pinAnalogico);
  tension = (lectura * 5.0f) / 1024.0f;
  anguloGirado = map(lectura, 0, 1023, 0, 180);

  Serial.print("Lectura potencimetro:\t");
  Serial.print(lectura);
  Serial.print("\tLectura voltaje:\t");
  Serial.print(tension);

  Serial.print("\tAngulo girado: \t");
  Serial.println(anguloGirado);

  primerServo.write(anguloGirado);
}

