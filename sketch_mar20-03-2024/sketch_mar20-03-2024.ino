#include <Arduino.h>
#include <math.h>
#include <IRremote.hpp>

#define LATCH 3
#define dataINPUT 2
#define CLOCK 4
#define BOTON 7
#define INFRARROJO 11

// 7 segment display controlado por un 74HC595, Q1-Q7 son las salidas del 74HC595

//        BgfabcdeP (P -> dp)
#define a B00000000

#define CERO B01111110    // Display 0
#define UNO B00011000     // Display 1
#define DOS B10110111     // Display 2
#define TRES B10111100    // Display 3
#define CUATRO B11011000  // Display 4
#define CINCO B11101100   // Display 5
#define SEIS B11101110    // Display 6
#define SIETE B00111000   // Display 7
#define OCHO B11111110    // Display 8

int numeroAleatorio = -1;
int valorBotonAnterior = 0;
int lecturaInfrarrojo = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LATCH, OUTPUT);
  pinMode(dataINPUT, OUTPUT);
  pinMode(CLOCK, OUTPUT);
  pinMode(BOTON, INPUT);
  IrReceiver.begin(INFRARROJO);

  digitalWrite(LATCH, LOW);
  shiftOut(dataINPUT, CLOCK, MSBFIRST, a);
  digitalWrite(LATCH, HIGH);
  IrReceiver.resume();
}

void loop() {
  int valorBoton = digitalRead(BOTON);

    
    if (IrReceiver.decode()) {
      lecturaInfrarrojo = IrReceiver.decodedIRData.command;
      Serial.print("La trama de 32 bits recibida es: ");
      Serial.println(IrReceiver.decodedIRData.decodedRawData, BIN);
      Serial.print("El comando recibido en DEC es: ");
      Serial.println(lecturaInfrarrojo, DEC);
      IrReceiver.printIRResultShort(&Serial);
      IrReceiver.resume();
    }

    
    

  // put your main code here, to run repeatedly:
  if ((valorBoton == HIGH && valorBotonAnterior == LOW) || lecturaInfrarrojo == 69) {

    NUMrandom();

    animacion();

    encenderNumero();
    Serial.print("Numero aleatorio: ");
    Serial.println(numeroAleatorio);

    lecturaInfrarrojo = -1;
    IrReceiver.resume();
  }

  // Serial.println(valorBoton);
  valorBotonAnterior = valorBoton;
  delayMicroseconds(500);

  
  
}

void NUMrandom() {
  randomSeed(analogRead(A0));
  numeroAleatorio = random(0, 9);
}

// Encendemos cada segmento del display (menos el g) a modo de ruleta
void animacion() {
  for (int j = 0; j < 5; j++) {
    for (int i = 1; i < 7; i++) {
      digitalWrite(LATCH, LOW);  // Paramos la salida
      // Las potencias de 2 son encienden solo un segmento
      shiftOut(dataINPUT, CLOCK, MSBFIRST, pow(2, i) + 1);
      digitalWrite(LATCH, HIGH);
      delay(50);
    }
  }
}

void encenderNumero() {
  switch (numeroAleatorio) {
    case 0:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, CERO);
      digitalWrite(LATCH, HIGH);
      break;
    case 1:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, UNO);
      digitalWrite(LATCH, HIGH);
      break;
    case 2:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, DOS);
      digitalWrite(LATCH, HIGH);
      break;
    case 3:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, TRES);
      digitalWrite(LATCH, HIGH);
      break;
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, TRES);
      digitalWrite(LATCH, HIGH);
      break;
    case 4:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, CUATRO);
      digitalWrite(LATCH, HIGH);
      break;
    case 5:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, CINCO);
      digitalWrite(LATCH, HIGH);
      break;
    case 6:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, SEIS);
      digitalWrite(LATCH, HIGH);
      break;
    case 7:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, SIETE);
      digitalWrite(LATCH, HIGH);
      break;
    case 8:
      digitalWrite(LATCH, LOW);  // Paramos la salida
      shiftOut(dataINPUT, CLOCK, MSBFIRST, OCHO);
      digitalWrite(LATCH, HIGH);
      break;
    default:
      break;
  }
}