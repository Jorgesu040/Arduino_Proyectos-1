
unsigned int entero = 0;
float flotante;
bool booleano;


void setup() {
  // put your setup code here, to run once:
Serial.begin (9600); //Esta es la velocidad baud (cambios de señal por segundo)

Serial.println("Hola mundo");

}

void loop() {
  // put your main code here, to run repeatedly:
  entero++;
 Serial.println(entero);
 delay (1000);
}
