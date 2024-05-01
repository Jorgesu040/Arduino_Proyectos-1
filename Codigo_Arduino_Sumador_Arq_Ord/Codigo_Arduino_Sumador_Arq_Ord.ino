/*
Autor: Jorge Matesanz
Fecha: 25/04/2024
Desc.: Sumador de 4 bits con dos modos de operacion: manual y automatico
*/

#include <Arduino.h>

#define bitA0 2
#define bitA1 3
#define bitA2 4
#define bitA3 5

#define bitB0 6
#define bitB1 7
#define bitB2 8
#define bitB3 9

int modo = 0;

void sumaManual();
void sumaAutomatica();

void setup() {
  // Setup de los pines de salida y de la comunicacion por el puerto serie
  Serial.begin(9600);
  pinMode(bitA0, OUTPUT);
  pinMode(bitA1, OUTPUT);
  pinMode(bitA2, OUTPUT);
  pinMode(bitA3, OUTPUT);

  pinMode(bitB0, OUTPUT);
  pinMode(bitB1, OUTPUT);
  pinMode(bitB2, OUTPUT);
  pinMode(bitB3, OUTPUT);

  // Seleccion del modo de operacion
  do {
    Serial.println("Selecciona el modo de operacion: ");
    Serial.println("1. Suma manual por terminal");
    Serial.println("2. Suma automatica");
    while (Serial.available() == 0); // Esperar el input del usuario
    modo = Serial.parseInt();
    while (Serial.available() > 0) Serial.read(); // Limpiar el buffer de entrada
    if (modo == 1) {
      Serial.println("Modo manual");
      break; 
    } else if (modo == 2) {
      Serial.println("Modo automatico");
      break; 
    }
    
  } while (true); // Loop infinito hasta que se seleccione un modo valido
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch (modo) {
    case 1:
      sumaManual();
      break;
    case 2:
      sumaAutomatica();
      break;
  }

}

void sumaManual(){
  int a = 0;
  int b = 0;
  int resultado = 0;
  bool error = false;

  do {
    error = false;
    /*Lectura del valor a*/
    while (Serial.available() > 0) 
      Serial.read(); // Limipar el buffer de entrada
    
    Serial.print("Introduce el valor de A (0-15): ");

    while (Serial.available() == 0); // Esperar el input del usuario
    a = Serial.parseInt(); // Leer el valor introducido por el usuario

    while (Serial.available() > 0) 
      Serial.read(); // Limpiar el buffer de entrada
    
    Serial.println(a);
    delay(100);

    /*Lectura del valor b*/
    Serial.print("Introduce el valor de B (0-15): ");

    while (Serial.available() == 0);
    b = Serial.parseInt();

    while (Serial.available() > 0) 
      Serial.read(); // Limpiar el buffer de entrada
    
    Serial.println(b);

    /*Comprobamos que los valores introducidos esten en el rango 0-15*/
    if (a < 0 || a > 15 || b < 0 || b > 15) {
      Serial.println("Valores invalidos. Introduce valores entre 0 y 15.");
      error = true;
    }
  } while (error);

  // Para cada numero separamos los bits y los enviamos a las salidas
  // Realizamos & con 0b01 para obtener el bit menos significativo (despues de haber desplazado cada cifra)
  digitalWrite(bitA0, a & 0b01);
  digitalWrite(bitA1, (a >> 1) & 0b01);
  digitalWrite(bitA2, (a >> 2) & 0b01);
  digitalWrite(bitA3, (a >> 3) & 0b01);

  digitalWrite(bitB0, b & 0b01);
  digitalWrite(bitB1, (b >> 1) & 0b01);
  digitalWrite(bitB2, (b >> 2) & 0b01);
  digitalWrite(bitB3, (b >> 3) & 0b01);


  resultado = a + b;
  Serial.print("Resultado: ");
  Serial.println(resultado);

}

void sumaAutomatica(){
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      // Para cada numero separamos los bits y los enviamos a las salidas
      // Realizamos & con 0b01 para obtener el bit menos significativo (despues de haber desplazado cada cifra)
      digitalWrite(bitA0, i & 0b01);
      digitalWrite(bitA1, (i >> 1) & 0b01);
      digitalWrite(bitA2, (i >> 2) & 0b01);
      digitalWrite(bitA3, (i >> 3) & 0b01);

      digitalWrite(bitB0, j & 0b01);
      digitalWrite(bitB1, (j >> 1) & 0b01);
      digitalWrite(bitB2, (j >> 2) & 0b01);
      digitalWrite(bitB3, (j >> 3) & 0b01);

      Serial.print("Sumando: ");
      Serial.print(i);
      Serial.print(" + ");
      Serial.print(j);
      Serial.print(" = ");
      Serial.println(i + j);
      delay(500);
    }
  }
  
}