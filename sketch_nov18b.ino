#include <DHT.h>
#include <stdio.h>

#define PINHUMEDAD 7
#define PINFLAMA A0
#define DHTTYPE DHT11
#define PINHUMO A1

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

float humedad;
float temperatura;
float hic;
float flama;
float rs_med;
float concentracion;

DHT dht(PINHUMEDAD, DHTTYPE);


void setup()
{
  dht.begin();
  pinMode(PINFLAMA, INPUT);
  Serial.begin(9600);
}

void loop()
{
  rs_med = readMQ(PINHUMO); 
  concentracion = getConcentration(rs_med/R0);
  Serial.print("Concentracion de gas : ");
  Serial.println(concentracion);
  
  flama = analogRead(PINFLAMA);
  Serial.print("Intensidad de flama : ");
  Serial.println(flama);
  
  //Recoger datos DHT
  humedad = dht.readHumidity();
  temperatura = dht.readTemperature();
  hic = dht.computeHeatIndex(temperatura,humedad,false);
  // Impresion de datos de DHT
  Serial.print("Humedad : " );
  Serial.print(humedad);
  Serial.print(" , Temperatura : ");
  Serial.print(temperatura);
  Serial.print(" , Indice de calor : ");
  Serial.print(hic);
  Serial.println(" ");

  delay(500);
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
