
#define pin1 2
#define pin2 3
#define pin3 4
#define pin4 5
#define pin5 6
#define pin6 7

#define relojInput 10 

unsigned int tiempo;
bool estadoActual;
bool ultimoEstado = 0 ;

void setup() {
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  for(int i = 2; i < 8; i++){
    pinMode(i, OUTPUT);
  }

  pinMode(relojInput, INPUT);

}

void loop() {
  estadoActual = digitalRead(relojInput);
  
  if (estadoActual != ultimoEstado) {
    if(estadoActual == HIGH){
      encenderLeds();
    } else {
      apagarLeds();
    }
    ultimoEstado = estadoActual;
  }
}


void encenderLeds(){
  unsigned long inicioTiempo = millis();
  while(millis() - inicioTiempo < 1400){
  tiempo = millis() - inicioTiempo;

  if(tiempo > 200){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
    digitalWrite(pin4, LOW);
    digitalWrite(pin5, LOW);
    digitalWrite(pin6, LOW);
  }
    if(tiempo > 400){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, LOW);
    digitalWrite(pin4, LOW);
    digitalWrite(pin5, LOW);
    digitalWrite(pin6, LOW);
  }
    if(tiempo > 600){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, LOW);
    digitalWrite(pin5, LOW);
    digitalWrite(pin6, LOW);
  }
    if(tiempo > 800){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, HIGH);
    digitalWrite(pin5, LOW);
    digitalWrite(pin6, LOW);
  }
    if(tiempo > 1000){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, HIGH);
    digitalWrite(pin5, HIGH);
    digitalWrite(pin6, LOW);
  }
  if(tiempo > 1200){
    digitalWrite(pin1, HIGH);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, HIGH);
    digitalWrite(pin5, HIGH);
    digitalWrite(pin6, HIGH);
  }
}
}

void apagarLeds(){
  unsigned long inicioTiempo = millis();
  while(millis() - inicioTiempo < 2800){
    tiempo = millis() - inicioTiempo;


  if(tiempo > 400){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, HIGH);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, HIGH);
    digitalWrite(pin5, HIGH);
    digitalWrite(pin6, HIGH);
  }
  if(tiempo > 800){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, HIGH);
    digitalWrite(pin4, HIGH);
    digitalWrite(pin5, HIGH);
    digitalWrite(pin6, HIGH);
  }
  if(tiempo > 1200){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
    digitalWrite(pin4, HIGH);
    digitalWrite(pin5, HIGH);
    digitalWrite(pin6, HIGH);
  }
  if(tiempo > 1600){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
    digitalWrite(pin4, LOW);
    digitalWrite(pin5, HIGH);
    digitalWrite(pin6, HIGH);
  }
  if(tiempo > 2000){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
    digitalWrite(pin4, LOW);
    digitalWrite(pin5, LOW);
    digitalWrite(pin6, HIGH);
  }

  if(tiempo > 2400){
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    digitalWrite(pin3, LOW);
    digitalWrite(pin4, LOW);
    digitalWrite(pin5, LOW);
    digitalWrite(pin6, LOW);
  }
}
}