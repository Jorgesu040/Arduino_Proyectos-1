#define pinAnalogico A0

const int led2 = 2;
const int led4 = 4;
const int led6 = 6;
const int led8 = 8;
const int led10 = 10;

int lectura;
float tension;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(led2, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(led6, OUTPUT);
  pinMode(led8, OUTPUT);
  pinMode(led10, OUTPUT);
  pinMode(pinAnalogico, INPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  lectura = analogRead(pinAnalogico);
  delay(1);

  tension = (lectura * 5.0f)/1024.0f;
  
  Serial.print("Lectura potencimetro:\t");
  Serial.print(lectura);
  Serial.print("\tLectura voltaje:\t");
  Serial.println(tension);
    if (tension >= 4.5) {

    digitalWrite(led2, HIGH);
    digitalWrite(led4, HIGH);
    digitalWrite(led6, HIGH);
    digitalWrite(led8, HIGH);
    digitalWrite(led10, HIGH);
  }
  else if (tension >= 4.0) {
  
  
    digitalWrite(led2, HIGH);
    digitalWrite(led4, HIGH);
    digitalWrite(led6, HIGH);
    digitalWrite(led8, HIGH);
    digitalWrite(led10, LOW);
  }
  else if (tension >= 3.0) {
 
  
    digitalWrite(led2, HIGH);
    digitalWrite(led4, HIGH);
    digitalWrite(led6, HIGH);
    digitalWrite(led8, LOW);
    digitalWrite(led10, LOW);
  }
  else if (tension >= 2.0) {

 
    digitalWrite(led2, HIGH);
    digitalWrite(led4, HIGH);
    digitalWrite(led6, LOW);
    digitalWrite(led8, LOW);
    digitalWrite(led10, LOW);
  }
  else if (tension >= 1.0){
   

    digitalWrite(led2, HIGH);
    digitalWrite(led4, LOW);
    digitalWrite(led6, LOW);
    digitalWrite(led8, LOW);
    digitalWrite(led10, LOW);
  }
    else if (tension >= 0.0){
    
 
    digitalWrite(led2, HIGH);
    digitalWrite(led4, LOW);
    digitalWrite(led6, LOW);
    digitalWrite(led8, LOW);
    digitalWrite(led10, LOW);

  }
}
