int velocidades = 0;
void setup()
{
  Serial.begin(9600);
  Serial.println("\nIntroduzca el Numero de veocidades seleccionables (maximo 9):");
  while (Serial.available() < 1)
  {
  } // Espera a que se introduzca un valor

  velocidades = Serial.read() - '0'; // Se resta 48 para convertir el valor ASCII a decimal

  if (velocidades < 1)
  {
    velocidades = 1;
  }

  if (velocidades > 9)
  { // Por si se introduce un caracter ASCII distinto
    velocidades = 9;
  }

  Serial.print("Numero de velocidades seleccionado: ");
  Serial.println(velocidades);
}

int botonSubir = 0, botonSubirPrevio = 0;
int botonBajar = 0, botonBajarPrevio = 0;

int vActual = 0;

unsigned long int desfase = 0;

void loop() {

  botonBajar = digitalRead(3);
  botonSubir = digitalRead(5);


  if (millis() - desfase < 2000){
    
    Serial.print(botonBajar);
  Serial.println(botonSubir);

    if(botonSubir == LOW && botonSubirPrevio == HIGH) {

      vActual++;
      if (vActual > velocidades){
        vActual = 0;
      }
      Serial.print("Velocidad actual: ");
      Serial.println(vActual);
      setVelocidad(vActual, velocidades);
      desfase = millis();
    }

    if(botonBajar == LOW && botonBajarPrevio == HIGH) {

      vActual--;
      if (vActual < 0){
        vActual = velocidades;
      }
      Serial.print("Velocidad actual: ");
      Serial.println(vActual);
      setVelocidad(vActual, velocidades);
      desfase = millis(); 
    }

    botonSubirPrevio = botonSubir; // Update previous state
    botonBajarPrevio = botonBajar; // Update previous state
  }
}

// Funcion que calcula el envio de la señal PWM por el pin 11 en funcion de la velocidad seleccionada
void setVelocidad(int vActual, int velocidades){
  byte dutyCycle = 0;
  if (vActual == 0){
    dutyCycle = 0;
  }
  else
    dutyCycle = (vActual * 255) / velocidades;
  analogWrite(11, dutyCycle);
}

