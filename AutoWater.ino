#include "st7783/TFTLCD.h"
#include <SD.h>
#include "st7783/TouchScreen.h"
//#include "reg51.h"

//#define PUMP_DIRECTION_PIN 2 //Waterpump direction, connected to B_IB pin on HG7881 (L9110) Dual Channel Motor Driver Module
#define PUMP_PWM_PIN 2 //Waterpump speed, connected to B_IA pin on HG7881 (L9110) Dual Channel Motor Driver Module
#define PWM_SLOW 50
#define PWM_FAST 200 

#define SOIL_SENSOR_PIN A5
#define WATER_SENSOR_PIN A4

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

// Colors
#define	BLACK           0x0000
#define	BLUE            0x001F
#define	RED             0xF800
#define	GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0 
#define WHITE           0xFFFF

#define VALUE_COUNT 24

TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET); //TFT chip 7783
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

int soilValues[VALUE_COUNT];
int counter = 0;

File root;

void setup(void) { 
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
  
  for (int i = 0; i < VALUE_COUNT; i++)
  {
    soilValues[i] = 0;
  }

  //pinMode(PUMP_DIRECTION_PIN, OUTPUT);
  pinMode(PUMP_PWM_PIN, OUTPUT);
  //digitalWrite(PUMP_DIRECTION_PIN, LOW);
  digitalWrite(PUMP_PWM_PIN, LOW);
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
  soilValues[counter] = int(soil_sensor);
  String soil_sensor_value = String(soil_sensor);  
  char soilSensorPrintout[4];
  soil_sensor_value.toCharArray(soilSensorPrintout, 4);

  if (soil_sensor > 100.0)
  {
    //digitalWrite(PUMP_DIRECTION_PIN, LOW);
    digitalWrite(PUMP_PWM_PIN, LOW);
    delay(1000);
    //digitalWrite(PUMP_DIRECTION_PIN, HIGH); // direction = forward
    analogWrite(PUMP_PWM_PIN, 255-PWM_FAST); // PWM speed = fast
  } else
  {
    //digitalWrite(PUMP_DIRECTION_PIN, LOW);
    digitalWrite(PUMP_PWM_PIN, LOW);
  }
  
  float water_sensor = analogRead(WATER_SENSOR_PIN);
  String water_sensor_value = String(water_sensor);  
  char waterSensorPrintout[4];
  water_sensor_value.toCharArray(waterSensorPrintout, 4);
  
  //Draw to screen
  tft.fillScreen(BLACK);
  
  tft.drawRect(tft.TFTWIDTH, 0, tft.TFTWIDTH, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(0, 0+2, "Soil:", CYAN);
  tft.drawString(80, 0+2, soilSensorPrintout, WHITE);
  tft.drawString(120, 0+2, "Water:", CYAN);
  tft.drawString(200, 0+2, waterSensorPrintout, WHITE);
  
  //float previous = soilValues[0];
  for (int i = 1; i < VALUE_COUNT; i++)
  {
    //tft.drawLine((i-1)*(tft.TFTWIDTH/VALUE_COUNT), tft.TFTHEIGHT/4-(previous/700)*(tft.TFTHEIGHT/4),i*(tft.TFTWIDTH/VALUE_COUNT), tft.TFTHEIGHT/4-(soilValues[i]/700)*(tft.TFTHEIGHT/4));
    //previous = soilValues[i];
  }
  
  tft.drawRect(tft.TFTWIDTH, tft.TFTHEIGHT/4, tft.TFTWIDTH, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(0, tft.TFTHEIGHT/4+2, "Manual water:", CYAN);
  
  tft.drawRect(tft.TFTWIDTH, tft.TFTHEIGHT/2, tft.TFTWIDTH, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(0, tft.TFTHEIGHT/2+2, "Watering level:", CYAN);
  //tft.drawString(80, tft.TFTHEIGHT/2+2, temperatureSensorPrintout, WHITE);
  
  tft.drawRect(tft.TFTWIDTH, 3*tft.TFTHEIGHT/4, tft.TFTWIDTH, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(0, 3*tft.TFTHEIGHT/4+2, "Disable watering:", CYAN);
  //tft.drawString(80, 3*tft.TFTHEIGHT/4+2, lightSensorPrintout, WHITE);

  
  
  delay(2000);
}

