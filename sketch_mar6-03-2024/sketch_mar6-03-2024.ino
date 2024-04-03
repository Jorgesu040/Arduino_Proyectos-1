#define LATCH 3
#define dataINPUT 2
#define CLOCK 4

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LATCH, OUTPUT);
  pinMode(dataINPUT, OUTPUT);
  pinMode(CLOCK, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  conteo();
}

void conteo(){
  for (int num = 0; num < 256; num++){
    digitalWrite(LATCH, LOW); // Paramos la salida
    shiftOut(dataINPUT, CLOCK, MSBFIRST, num);
    digitalWrite(LATCH, HIGH);
    Serial.println(num);
    delay(500);
  }


}
