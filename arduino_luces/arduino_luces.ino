#define PINSONIDO A0
#define PINLED 7
#define PINLDR A1

float sonido;
bool value= true;
float luz;
void setup() {
 
 Serial.begin(9600);
}

void loop() {
  luz = analogRead(PINLDR);

  Serial.print("La luz es de ");
  Serial.println(luz);
  sonido = analogRead(PINSONIDO);
  
  Serial.print("El sonido es ");
  Serial.println(sonido);
  
  if(sonido>100)
  {
    value = !value;
  }

  /*if(luz>300)
  {
    value= false;
  }*/
  digitalWrite(PINLED,value);

  delay(500);
}
