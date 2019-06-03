// automated watering for plants with metering of the soil moisture and data logging
// (c) Jan Mazanec, jmazanec@gmail.com, 23.5.2019
// Arduino UNO; SD card shield with RTC; water pumpsô relaysô DHT22, capacitance soil humidity meter

#define PUMP1 6 
#define PUMP2 7 
#define PUMP3 8 
#define PUMP4 9

#define HUM_METER1 A0 
#define HUM_METER2 A1 
#define HUM_METER3 A2

#define WATERING_HOUR1 6

#define THRESHOLD_MOISTURE1 280 
#define THRESHOLD_MOISTURE2 285
#define THRESHOLD_MOISTURE3 300
#define THRESHOLD_MOISTURE_MAX 800  

#include "DHT.h"
#define DHTPIN            2         // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

int time_pump1=12;
int time_pump2=13;
int time_pump3=12;
int time_pump4=12;

int moisture1;
int moisture2;
int moisture3;

// SDshield tutorial:
// https://navody.arduino-shop.cz/navody-k-produktum/data-logger-shield.html
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include "RTClib.h"
#include <Time.h>

RTC_DS1307 DS1307;
// CS pin for SD card - fixed for this shield
const int sd_CS = 10;

// bytes needed: 7-time ; 5-temp ; 5-hum ; 4x2+3=11-watering pump1-4; 3*4+2 moist1-3  = 46
char dataString [50];

int startup = 1;


void setup() {
  pinMode(PUMP1, OUTPUT);
  pinMode(PUMP2, OUTPUT);
  pinMode(PUMP3, OUTPUT);
  pinMode(PUMP4, OUTPUT);
  digitalWrite(PUMP1, HIGH); 
  digitalWrite(PUMP2, HIGH);
  digitalWrite(PUMP3, HIGH);
  digitalWrite(PUMP4, HIGH);
  Serial.begin(9600);
  // put your setup code here, to run once:
  dht.begin();
  // check if time clock is attached
  if (! DS1307.begin()) {
    Serial.println(F("Clock not found"));
    while (1);
  }
  // check if time clock is running
  if (! DS1307.isrunning()) {
    Serial.println(F("Clock not running! Starting..."));
  }
  // check of SD card
  if (!SD.begin(sd_CS)) {
    Serial.println(F("SD card not available or broken!"));
    return;
  }
}


void loop() {
  DateTime current_time = DS1307.now();
  // read temp and humidity
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  //read moistures
  moisture1 = analogRead(HUM_METER1);
  moisture2 = analogRead(HUM_METER2);
  moisture3 = analogRead(HUM_METER3);
  int time_watering_pump1=0;
  int time_watering_pump2=0;
  int time_watering_pump3=0;
  int time_watering_pump4=0;

  // at startup we water the plants once
  if (startup==1 || (current_time.hour()==WATERING_HOUR1 && current_time.minute()<2)) {
    startup=0;

    if (moisture1>THRESHOLD_MOISTURE1 && moisture1<THRESHOLD_MOISTURE_MAX && time_pump1>0) {
      Serial.println("watering1");
      time_watering_pump1=time_pump1;
      digitalWrite(PUMP1, LOW);
      delay(1000*time_watering_pump1);
      digitalWrite(PUMP1, HIGH);
    }

    if (moisture2>THRESHOLD_MOISTURE2 && moisture2<THRESHOLD_MOISTURE_MAX && time_pump2>0) {
      Serial.println("watering2");
      time_watering_pump2=time_pump2;
      digitalWrite(PUMP2, LOW);
      delay(1000*time_watering_pump2);
      digitalWrite(PUMP2, HIGH);
    }

    if (moisture3>THRESHOLD_MOISTURE3  && moisture3<THRESHOLD_MOISTURE_MAX && time_pump3>0) {
      Serial.println("watering3");
      time_watering_pump3=time_pump3;
      digitalWrite(PUMP3, LOW);
      delay(1000*time_watering_pump3);
      digitalWrite(PUMP3, HIGH);
    }  

    if (moisture3>THRESHOLD_MOISTURE3  && moisture3<THRESHOLD_MOISTURE_MAX && time_pump4>0) {
      Serial.println("watering4");
      time_watering_pump4=time_pump4;
      digitalWrite(PUMP4, LOW);
      delay(1000*time_watering_pump4);
      digitalWrite(PUMP4, HIGH);
    }

    delay(60000);
  }

  
  char filename [13];
  // filename in format YEARMMDD.csv
  sprintf (filename, "%04u%02u%02u.csv", current_time.year(), current_time.month(), current_time.day());
      
  sprintf (dataString, "%02u:%02u:%02u;%d.%d;%d.%d;%d;%d;%d;%d;%d;%d;%d", current_time.hour(), current_time.minute(), current_time.second(),(int)t,(int)(t*10)%10,(int)h,(int)(h*10)%10,time_watering_pump1,time_watering_pump2,time_watering_pump3,time_watering_pump4,moisture1,moisture2,moisture3);
  // put your main code here, to run repeatedly:
  Serial.println(dataString);
 
  File dataWrite = SD.open(filename, FILE_WRITE);
  if (dataWrite) {
        dataWrite.println(dataString);
        dataWrite.close();
        Serial.println(F("Write on SD successfull"));
      }
      // when problem writing
      else {
        Serial.print(F("Error opening file "));
        Serial.println(filename);
      }
  delay(60000);
}
