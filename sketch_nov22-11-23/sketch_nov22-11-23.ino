#define pinZumbador 9
#define pinFotoresistencia A0
#define pinBoton 5 

void setup() {
  pinMode(pinZumbador, OUTPUT);
  pinMode(pinFotoresistencia, INPUT);  
  pinMode(pinBoton, INPUT);
  Serial.begin(9600);
}

void loop() {
  int valorLuz = analogRead(pinFotoresistencia);
  // Ajusta la duración de la nota según la luz
  int duracionNota = map(valorLuz, 0, 1023, 100, 500);  
  Serial.println(digitalRead(pinBoton));

  if (digitalRead(pinBoton) == HIGH) {  // Si el botón está pulsado
    tocarSOS(duracionNota);  // Toca el SOS
  }
  else{
   for (int i = 0; i < 3; i++) {
    tocarNota(293.66, duracionNota);
    delay(100);
  }

  for (int i = 0; i < 4; i++) {
    tocarNota(440, duracionNota);
    delay(100);
  }

  for (int i = 0; i < 4; i++) {
    tocarNota(523.25, duracionNota);
    delay(100);
  }
  }
  delay(1000);  // Espera antes de comenzar de nuevo
}

void tocarNota(float frecuencia, int duracion) {
  tone(pinZumbador, frecuencia, duracion);
  delay(duracion + 50);  // Pequeña pausa entre cada nota
  noTone(pinZumbador);
}

void tocarSOS(int duracionNota) {
  // SOS en código Morse es ...---
  for (int i = 0; i < 3; i++) {  // Toca 3 notas cortas
    tocarNota(440, duracionNota);
    delay(duracionNota);
  }
  delay(duracionNota * 3);  // Pausa entre las notas cortas y las largas
  for (int i = 0; i < 3; i++) {  // Toca 3 notas largas
    tocarNota(440, duracionNota * 3);
    delay(duracionNota);
  }
  delay(duracionNota * 3);  // Pausa entre las notas largas y las cortas
  for (int i = 0; i < 3; i++) {  // Toca 3 notas cortas
    tocarNota(440, duracionNota);
    delay(duracionNota);
  }
}
