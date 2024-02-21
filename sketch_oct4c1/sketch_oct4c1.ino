

//Reservo espacio para dos variables 
char opcion;
int led = 10;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
  //configuro el pin 10 
pinMode(led, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Si esta disponible para recibir instrucciones, comienza el condicional
  if(Serial.available()> 0){
    opcion = Serial.read();
    //encendido
    if (opcion == 'e' || opcion == 'E'){
       digitalWrite (10, HIGH);
    //apagado
    } else if (opcion == 'a' || opcion == 'A')
      digitalWrite (10, LOW);

  } 

}
