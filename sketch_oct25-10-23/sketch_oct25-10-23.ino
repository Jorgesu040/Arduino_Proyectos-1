  #define pinAnalogico A1

  const int led2 = 2;
  const int led4 = 4;
  const int led6 = 6;


  float resT;
  float vout;
  int lectura;
  float sensibilidad;
  float resM;
  float temperatura;

  void setup() {
    // put your setup code here, to run once:
    Serial.begin(9600);
    pinMode(pinAnalogico, INPUT);
    pinMode(led2, OUTPUT);
    pinMode(led4, OUTPUT);
    pinMode(led6, OUTPUT);
    
    sensibilidad = (-10071.0 + 7150.0) / 8.0;
  }

  void loop() {

    // put your main code here, to run repeatedly:
    resM = medidaRT(resT, vout, lectura);

    temperatura =  22.0  + (resM - 10071.0)/ sensibilidad;  
    Serial.println (temperatura);

    if(temperatura < 23.0){
      digitalWrite(led2, LOW);
      digitalWrite(led4, LOW);
      digitalWrite(led6, LOW);

    } else if (temperatura >= 23.0 && temperatura <= 25.0){
      digitalWrite(led2, HIGH);
      digitalWrite(led4, LOW);
      digitalWrite(led6, LOW);

    } else if (temperatura > 27.0 && temperatura <= 29.0){
      digitalWrite(led2, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led6, LOW);

    } else if (temperatura > 29.0){
      digitalWrite(led2, HIGH);
      digitalWrite(led4, HIGH);
      digitalWrite(led6, HIGH);

    }
    encenderLed(temperatura);
  }

  float medidaRT(float resT, float vout, int lectura){
    lectura = analogRead(pinAnalogico);
    vout = lectura * 5.0 / 1024.0;
    resT = 10000.0 * (5.0/vout - 1.0);
    
  //10071
  //7150
    return resT;
  }

int encenderLed(float temperatura){
  

  return 0;
}