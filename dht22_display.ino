#include <Arduino.h>
#include <TM1637Display.h>
#include "DHT.h"
#include <Process.h>
#include <Bridge.h>

//Board connections
#define DHTPIN 2     
#define CLK 4
#define DIO 3

#define DHTTYPE DHT22

#define DELAY 1000

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
TM1637Display display(CLK, DIO);

float t,h,f,hi;
Process date;
int hours,minutes;
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
int nextSegment = 1;
String date_string;

void setup() {
  Bridge.begin();
  Serial.begin(9600); 
  Serial.println("DHT22 + DISPLAY");
 
  dht.begin();
  
  display.setBrightness(0x0a);
  
  //Check date for the first time
  date.begin("date");
  date.addParameter("+%H%M");
  date.runAsynchronously();
  date_string = date.readString();
}

void loop() {
  unsigned long currentMillis = millis();
  
  h = dht.readHumidity();
  // Read temperature as Celsius
  t = dht.readTemperature();
  // Read temperature as Fahrenheit
  f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // Compute heat index
  // Must send in temp in Fahrenheit!
  hi = dht.computeHeatIndex(f, h);
  
  //Check date here every 3 s
  if (currentMillis - previousMillis2 >= 3*DELAY && !date.running()) {
    previousMillis2=currentMillis;
    date.begin("date");
    date.addParameter("+%H%M");
    date.runAsynchronously();
    date_string = date.readString();
  }
  
  if(currentMillis - previousMillis >= DELAY) {
    previousMillis = currentMillis;
    
    //Show temperature, humidity and clock in TM1637
    if(nextSegment == 1) {
      display.showNumberDec(t,false);
      nextSegment = 2;
    }else if (nextSegment == 2) {
      display.showNumberDec(h,false);
      nextSegment = 3;
    }else if (nextSegment == 3) {
      //show date
      display.showNumberDec(date_string.toInt(),true);
      //Light-up middle dots
      //http://forum.dev.arduino.cc/index.php?topic=271238.0
      uint8_t segto;
      segto=0x80 | display.encodeDigit((date_string.toInt()/100)%10);
      display.setSegments(&segto,1,1);

      nextSegment = 1;
    }
    
    //Display values in Serial
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
    
    //Pass values to Arduino Bridge
    Bridge.put("temperature", String(t));
    Bridge.put("humidity", String(h));
    Bridge.put("heatIndex", String(dht.convertFtoC(hi)));
  }
  
}
