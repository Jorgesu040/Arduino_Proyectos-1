int led9 = 9;

int led7 = 7;

int led5 = 5;

int pulsador = 2;

bool booleano;

bool consola;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  //configuro el pin 10 
pinMode(led9, OUTPUT);
pinMode(led7, OUTPUT);
pinMode(led5, OUTPUT);
pinMode(pulsador, INPUT);

Serial.println("Nave configurada!");



}

void loop() {
  // put your main code here, to run repeatedly:
  booleano = digitalRead(pulsador);

    
  if(booleano == 1){

      if(consola==0){
        Serial.println("Apagado");
        
      } else if (consola == 1) {
        Serial.println("Encendido");
        
      }
      

    digitalWrite(led5, LOW);


    digitalWrite(led9, HIGH);
    delay(250);
    digitalWrite(led9, LOW);
    delay(250);
    digitalWrite(led7, HIGH);
    delay(250);
    digitalWrite(led7, LOW);
    delay(250);

  } else if (booleano == 0) {
    digitalWrite(led9, LOW);
    digitalWrite(led7, LOW);
    digitalWrite(led5, HIGH);

  }
    consola = digitalRead(led5);
  }

