#include <DHT.h>
#include <SoftwareSerial.h>

#define PINHUMEDAD 7
#define PINFLAMA A0
#define DHTTYPE DHT11
#define PINHUMO A1
#define PINBUZZER 8

#define PINSONIDO A5
#define PINLDR A4
#define PINLED 9


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
float luz;

bool controlsonido=false;

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
  pinMode(PINLDR, INPUT);

        
  BT.begin(9600);
  Serial.begin(9600);

  BT.println("Bluetooth encendido");
  BT.println("");
}

void loop()
{

  
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

  //prender buzzer
  if((concentracion>200000 && hic>50) ||  (flama<300 && hic>50))
  {
    digitalWrite(PINBUZZER, HIGH);
  }
  else
  {
    digitalWrite(PINBUZZER,LOW);
  }


  //LDR

  luz = analogRead(PINLDR);
  /*Serial.print("La luz es de ");
  Serial.println(luz);*/

  //sonido
  sonido = analogRead(PINSONIDO);
  /*Serial.print("El sonido es ");
  Serial.println(sonido);*/
  
  if(sonido>800 && controlsonido==true)
  {
    value = !value;
  }
  
  digitalWrite(PINLED,value);

  if(BT.available())   
  {
     //Serial.write(BT.read());

     char  data=BT.read();

     if(data=='1')
     {
        BT.println("Sensor sonido prendido");
        BT.println(); 
        controlsonido=true; 
     }
     else if(data=='0')
     {
        BT.println("Sensor sonido apagado");
        BT.println();
        controlsonido=false;
     }

     if(data=='2')
     {
        BT.println("foco prendido");
        BT.println();
        value=true;
     }
     else if(data=='3')
     {
        BT.println("foco apagado");
        BT.println();
        value=false;
     }
  }

  String test =String(concentracion)+","+String(flama)+","+String(humedad)+","+String(temperatura)+","+String(hic)+","+String(luz);
  //BT.println(test);
  
  if(Serial.available())  
  {
    
    BT.write(Serial.read());

     
  }

  

}

// Obtener la resistencia promedio en N muestras
float readMQ(int mq_pin)
{
   float rs = 0;
   for (int i = 0;i<READ_SAMPLE_TIMES;i++) {
      rs += getMQResistance(analogRead(mq_pin));
      delay(READ_SAMPLE_INTERVAL);
   }
   return rs / READ_SAMPLE_TIMES;
}
 
// Obtener resistencia a partir de la lectura analogica
float getMQResistance(int raw_adc)
{
   return (((float)RL_VALUE / 1000.0*(1023 - raw_adc) / raw_adc));
}
 
// Obtener concentracion 10^(coord + scope * log (rs/r0)
float getConcentration(float rs_ro_ratio)
{
   return pow(10, coord + scope * log(rs_ro_ratio));
}
