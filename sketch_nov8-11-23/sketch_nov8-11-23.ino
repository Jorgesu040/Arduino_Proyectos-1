#define pinA0 A0
#define pinA2 A2
#define pinA4 A4

const int rojo = 9;
const int verde = 10;
const int azul = 11;

int readA0 = 0;
int readA1 = 0;
int readA2t = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(rojo, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(azul, OUTPUT);

  pinMode(pinA0, INPUT);
  pinMode(pinA2, INPUT);
  pinMode(pinA4, INPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  
  readA0 = analogRead(pinA0);
  readA1 = analogRead(pinA2);
  readA2t = analogRead(pinA4);

  int map1 = map(readA0, 54, 974, 0, 255);
  int map2 = map(readA1, 54, 974, 0, 255);
  int map3 = map(readA2t, 0, 1023, 0, 255);
  
  
  Serial.print(readA0);
  Serial.print("  readA0  ->");
  Serial.print(map1);
  Serial.print("  readA2  ->");
  Serial.print(map2);
  Serial.print("  readA2  ->");
  Serial.println(map3);
  
  analogWrite(rojo, map1);
  analogWrite(verde, map2);
  analogWrite(azul, map3);
}
