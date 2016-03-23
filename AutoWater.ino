//#include <DHT.h>
#include "TFTLCD.h"
#include <SD.h>
#include "TouchScreen.h"
#include <ArrayList.h>

//#define DHTPIN 2 //Yhsistä lämpötila/ilmankosteussensorin signaali DIGITAALI pinniin 0
//#define LIGHT_SENSOR_PIN 3//Yhdistä valosensorin signaali DIGITAALI pinniin 1
#define PUMP_PIN 2 //Yhdistä moottori (vesipumppu) B:n signaali DIGITAALI pinniin 2

//#define DHTTYPE DHT11 //Ilmankosteussensorin tyyppi on DHT111

//LCD Pins
#define LCD_CS A3    
#define LCD_CD A2    
#define LCD_WR A1   
#define LCD_RD A0    
#define LCD_RESET A4
#define SD_CS 10 

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define SOIL_SENSOR_PIN A5
#define WATER_SENSOR_PIN A4
//#define DHT_SENSOR_PIN 2
//#define LIGHT_SENSOR_PIN 3

#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF

int VALUE_COUNT = 24;
TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //TFT chip 7783
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);
//DHT dht(DHTPIN, DHTTYPE);

//ArrayList tempValues;
ArrayList soilValues;
//ArrayList humidityValues;
//ArrayList lightValues;

char soilSensorPrintout[4];
//char humiditySensorPrintout[4];
//char temperatureSensorPrintout[4];
//char lightSensorPrintout[4];
File root;

void setup(void) { 
  //tempValues = new ArrayList();
  soilValues = new ArrayList();
  //humidityValues = new ArrayList();
  //lightValues = new ArrayList();
  Serial.begin(9600);
  

  tft.reset();
 
  
  //Serial.print(F("Initializing SD card..."));
  //if (!SD.begin(SD_CS)) {
  //  Serial.println(F("failed!"));
  //  return;
  //}
  
  tft.initDisplay();  
  tft.fillScreen(CYAN);
  tft.setRotation(2);
  
  tft.setTextColor(BLACK);
  tft.setTextSize(100);
  
  for (int i = 1; i < VALUE_COUNT; i++)
  {
    //tempValues.add(0.0f);
    soilValues.add(0.0f);
    //humidityValues.add(0.0f);
    //lightValues.add(0.0f);
  }
}

void loop(void) { 
  
  //Check for press
  //TSPoint p = ts.getPoint();
  
  // we have some minimum pressure we consider 'valid'
  // pressure of 0 means no pressing!
  //if (p.z > ts.pressureThreshhold) {
  //   Serial.print("X = "); Serial.print(p.x);
  //   Serial.print("\tY = "); Serial.print(p.y);
  //   Serial.print("\tPressure = "); Serial.println(p.z);
  //}

  //Read sensor data
  float soil_sensor = analogRead(SOIL_SENSOR_PIN);
  String soil_sensor_value = String(soil_sensor);
  //float humidity = dht.readHumidity();
  //float temperature = dht.readTemperature();
  //boolean light = digitalRead(LIGHT_SENSOR_PIN);
  //String soil_sensor_value = "366";
  //String h = String(humidity);
  //String t = String(temperature);
  //String l = String(light);
  

  //tempValues.add(temperature);
  //humidityValues.add(humidity);
  //lightValues.add(light);
  soilValues.add(soil_sensor);
 
  if (soilValues.size() > VALUE_COUNT)
  {
    //tempValues.remove(0);
    soilValues.remove(0);
    //humidityValues.remove(0);
    //lightValues.remove(0);
  }

  // Check if any reads failed and exit early (to try again).
  //if (isnan(h) || isnan(t)) {
  //  Serial.println("Failed to read from DHT sensor!");
  //  return;
  //}
  //Serial.print("Humidity:"); 
  //Serial.print(h);
  //Serial.print(",");
  //Serial.print("Temperature:"); 
  //Serial.print(t);
  //Serial.print(",");
 
  // convert the readings to a char arrays
  soil_sensor_value.toCharArray(soilSensorPrintout, 4);
  //h.toCharArray(humiditySensorPrintout, 4);
  //t.toCharArray(temperatureSensorPrintout, 4);
  //l.toCharArray(lightSensorPrintout, 4);
  
  //Draw to screen
  tft.fillScreen(BLACK);
  
  tft.drawRect(tft.TFTWIDTH, 0, tft.TFTWIDTH, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(0, 0+2, "Soil:", CYAN);
  tft.drawString(80, 0+2, soilSensorPrintout, WHITE);
  float previous = soilValues[0];
  for (int i = 1; i < VALUE_COUNT; i++)
  {
    tft.drawLine((i-1)*(tft.TFTWIDTH/VALUE_COUNT), tft.TFTHEIGHT/4-(previous/700)*(tft.TFTHEIGHT/4),i*(tft.TFTWIDTH/VALUE_COUNT), tft.TFTHEIGHT/4-(soilValues[i]/700)*(tft.TFTHEIGHT/4));
    previous = soilValues[i];
  }
  
  tft.drawRect(tft.TFTWIDTH, tft.TFTHEIGHT/4, tft.TFTWIDTH, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(0, tft.TFTHEIGHT/4+2, "Humidity:", CYAN);
  //tft.drawString(80, tft.TFTHEIGHT/4+2, humiditySensorPrintout, WHITE);
  
  tft.drawRect(tft.TFTWIDTH, tft.TFTHEIGHT/2, tft.TFTWIDTH, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(0, tft.TFTHEIGHT/2+2, "Temperature:", CYAN);
  //tft.drawString(80, tft.TFTHEIGHT/2+2, temperatureSensorPrintout, WHITE);
  
  tft.drawRect(tft.TFTWIDTH, 3*tft.TFTHEIGHT/4, tft.TFTWIDTH, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(0, 3*tft.TFTHEIGHT/4+2, "Light:", CYAN);
  //tft.drawString(80, 3*tft.TFTHEIGHT/4+2, lightSensorPrintout, WHITE);
  
  delay(2000);
}

