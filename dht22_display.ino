#include <Arduino.h>
#include <TM1637Display.h>
#include "DHT.h"
#include <Process.h>


//Board connections
#define DHTPIN 2     
#define CLK 4
#define DIO 3

#define DHTTYPE DHT22   // DHT 22  (AM2302)

#define DELAY 1000

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);

//Temperature variable
float t,h;
//Variable for date
Process date;
int hours,minutes;
int lastSecond =-1;

void setup() {
  Bridge.begin();
  Serial.begin(9600); 
  Serial.println("DHT22 + DISPLAY");
 
  dht.begin();
  //Initial measurement of temperature
  t = dht.readTemperature();
  h = dht.readHumidity();
  
  //Get dat from network
  if (!date.running()) {
    date.begin("date");
    date.addParameter("+%H%M");
    date.run();
  }
}

void loop() {
  display.setBrightness(0x0a);
  
  //Display temperature on TM1637

  display.showNumberDec(t,false);

  delay(DELAY);
  display.showNumberDec(h,false);
  //display.showNumberDec(25,false);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius
  t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  


  // Compute heat index
  // Must send in temp in Fahrenheit!
  float hi = dht.computeHeatIndex(f, h);

  Serial.print("[");
  Serial.print(date.readString());
  Serial.print("]");
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hi);
  Serial.println(" *F");
  
  
  if (!date.running()) {
    date.begin("date");
    date.addParameter("+%H%M");
    date.run();
  }
  
  String s=date.readString();
  display.showNumberDec(s.toInt(),true);
  //Light-up middle dots
  //http://forum.dev.arduino.cc/index.php?topic=271238.0
  uint8_t segto;
  segto=0x80 | display.encodeDigit((s.toInt()/100)%10);
  display.setSegments(&segto,1,1);
  
}
