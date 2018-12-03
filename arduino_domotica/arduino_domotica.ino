#include <DHT.h>
#include <SoftwareSerial.h>

#define PINHUMEDAD 3
#define PINFLAMA A0
#define DHTTYPE DHT11
#define PINHUMO A1
#define PINBUZZER 4
#define PINLLUVIA A4

#define PINSONIDO A5
#define PINLED 5
#define PINLED1 6
#define PINLED2 7
#define PINLED3 8

#define PINALIMENTARSONIDO 2

#define LEDROJO 9



const int RL_VALUE = 5;
const int R0 = 10;   

const int READ_SAMPLE_INTERVAL = 100;
const int READ_SAMPLE_TIMES = 5;    

const float X0 = 200;
const float Y0 = 1.7;
const float X1 = 10000;
const float Y1 = 0.28;

const float punto0[] = { log10(X0), log10(Y0) };
const float punto1[] = { log10(X1), log10(Y1) };

const float scope = (punto1[1] - punto0[1]) / (punto1[0] - punto0[0]);
const float coord = punto0[1] - punto0[0] * scope;

//modulo fuego
float humedad;
float temperatura;
float hic;
float flama;
float rs_med;
float concentracion;

//modulo luces
float sonido;
bool value= true;
bool value1=true;
bool value2=true;
bool value3=true;


bool alimentar=true;
bool controlsonido=false;
float lluvia;
bool alarma;

SoftwareSerial BT(10, 11); // RX | TX

DHT dht(PINHUMEDAD, DHTTYPE);


void setup()
{
  dht.begin();
  pinMode(PINFLAMA, INPUT);
  pinMode(PINHUMO, INPUT);
  pinMode(PINBUZZER, OUTPUT);

  pinMode(PINSONIDO,INPUT);
  pinMode(PINLED,OUTPUT);
  pinMode(PINLED1,OUTPUT);
  pinMode(PINLED2,OUTPUT);
  pinMode(PINLED3,OUTPUT);

  pinMode(PINALIMENTARSONIDO, OUTPUT),

  pinMode(PINLLUVIA, INPUT);

  pinMode(LEDROJO, OUTPUT);

        
  BT.begin(9600);
  Serial.begin(9600);

  BT.println("Bluetooth encendido");
  BT.println("");

  
}

void loop()
{
  //String(concentracion)+","+String(flama)+","+String(humedad)+","+String(temperatura)+","+String(hic)+","+String(lluvia);
  String denviar[]={"0","0","0","0","0","0","0"};
  
  digitalWrite(LEDROJO, alarma);
  
  digitalWrite(PINALIMENTARSONIDO, controlsonido);
  rs_med = readMQ(PINHUMO); 
  concentracion = getConcentration(rs_med/R0);
  /*Serial.print("Concentracion de gas : ");
  Serial.println(concentracion);*/
  
  flama = analogRead(PINFLAMA);
  /*Serial.print("Intensidad de flama : ");
  Serial.println(flama);*/
  
  //Recoger datos DHT
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();
  hic = dht.computeHeatIndex(temperatura,humedad,false);
  // Impresion de datos de DHT
  /*Serial.print("Humedad : " );
  Serial.print(humedad);
  Serial.print(" , Temperatura : ");
  Serial.print(temperatura);
  Serial.print(" , Indice de calor : ");
  Serial.print(hic);
  Serial.println(" ");*/

  lluvia=analogRead(PINLLUVIA);
  //Serial.println(lluvia);
  //prender buzzer
  
  if((concentracion>70000) &&  (flama<300))
  {
    digitalWrite(PINBUZZER, HIGH);
    alarma=true; 
    denviar[6]="1";
  }
  else
  {
    digitalWrite(PINBUZZER,LOW);
    alarma=false;
  }

  //sonido
  sonido = analogRead(PINSONIDO);
  /*Serial.print("El sonido es ");
  Serial.println(sonido);*/
  
  if(sonido>800 && controlsonido==true)
  {
    value = !value;
  }
  
  digitalWrite(PINLED,value);
  digitalWrite(PINLED1,value1);
  digitalWrite(PINLED2,value2);
  digitalWrite(PINLED3,value3);

  if(BT.available())   
  {
     char  data=BT.read();

     if(data=='0')
     {
        alimentar=!alimentar;
     }
     else if(data=='1')
     {
        controlsonido=!controlsonido; 
     }
     else if(data=='2')
     {
        value=!value; 
     }
     else if(data=='3')
     {
        value1=!value1; 
     }
     else if(data=='4')
     {
        value2=!value2; 
     }
     else if(data=='5')
     {
        value3=!value3; 
     }
     else if(data=='6')
     {
        
        //Serial.flush();
     }
  }

  

  //datos enviados
  denviar[0]=concentracion;
  denviar[1]=flama;
  denviar[5]=lluvia;

  String test = denviar[6]+" "+denviar[0]+" "+denviar[1]+" "+String(humedad)+" "+String(temperatura)+" "+String(hic)+" "+denviar[5];
  BT.println(test);

  //BT.println(denviar[6]);

  //Serial.println(test);

  delay(300);
}

float readMQ(int mq_pin)
{
   float rs = 0;
   for (int i = 0;i<READ_SAMPLE_TIMES;i++) {
      rs += getMQResistance(analogRead(mq_pin));
      delay(READ_SAMPLE_INTERVAL);
   }
   return rs / READ_SAMPLE_TIMES;
}
 
float getMQResistance(int raw_adc)
{
   return (((float)RL_VALUE / 1000.0*(1023 - raw_adc) / raw_adc));
}

float getConcentration(float rs_ro_ratio)
{
   return pow(10, coord + scope * log(rs_ro_ratio));
}
