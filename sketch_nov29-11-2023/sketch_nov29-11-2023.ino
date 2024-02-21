#include <DHT.h>
#include <Servo.h> 

#define pinSensor 5
#define pinServo 9

Servo primerServo;
DHT sensorTemperatura(pinSensor, DHT11);

float temperatura, humedad;
int lectura; //Valor entre 0 y 1023
int anguloGirado;
int girarHumedad = -1;

void setup() {
  Serial.begin(9600);
  primerServo.attach(pinServo);
  sensorTemperatura.begin();
  Serial.print("Introduce un valor de humedad (0-100%) a partir de cual girar el servo: ");
}

void loop() {
  if(Serial.available() > 0){
    girarHumedad = Serial.read();
  }

  delay(2000);
  temperatura = sensorTemperatura.readTemperature();
  humedad = sensorTemperatura.readHumidity();
  
  anguloGirado = map(humedad, girarHumedad, 100, 0, 180);

  imprimirValores(temperatura, humedad);

  girarServoHumedad(girarHumedad, anguloGirado);
}

void imprimirValores(float temperatura, float humedad){
  Serial.print("La temperatura leida es: ");
  Serial.print(temperatura);
  Serial.print("ºC");
  Serial.print("\tLa humedad leida es: ");
  Serial.print(humedad);
  Serial.print("%");
}

void girarServoHumedad(int girarHumedad, int anguloGirado){
  Serial.print("\tAngulo girado: ");
  Serial.println(anguloGirado);
  if(girarHumedad > 80){
    girarHumedad = 80;
  }

  if(girarHumedad < 40){
    girarHumedad = 40;
  }

  if (humedad >= girarHumedad){
    primerServo.write(anguloGirado);
  }
}
