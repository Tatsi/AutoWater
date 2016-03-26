#include "st7783/TFTLCD.h"
//#include <SD.h>
#include "st7783/TouchScreen.h"
//#include "reg51.h"

//#define PUMP_DIRECTION_PIN 2 //Waterpump direction, connected to B_IB pin on HG7881 (L9110) Dual Channel Motor Driver Module
#define PUMP_PWM_PIN 2 //Waterpump speed, connected to B_IA pin on HG7881 (L9110) Dual Channel Motor Driver Module
#define PWM_SLOW 50
#define PWM_FAST 200 

#define SOIL_SENSOR_PIN A5

//LCD Pins
#define LCD_CS A3    
#define LCD_CD A2    
#define LCD_WR A1   
#define LCD_RD A0    
#define LCD_RESET A4
//#define SD_CS 10 

#define YP A1  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 7   // can be a digital pin
#define XP 6   // can be a digital pin

#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940

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
bool watering_disabled = false;
int watering_level = 5; // 1-5
int text_left_padding = tft.TFTWIDTH/4;
int button_padding = 10;
//File root;
char soilSensorPrintout[4];
int update_counter = 0;

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
  pinMode(13, OUTPUT);
  pinMode(PUMP_PWM_PIN, OUTPUT);
  digitalWrite(PUMP_PWM_PIN, LOW);
}

void loop(void) { 
  //drawScreen();
  //delay(2000);
  
  //Check for press
  //Point p;
  digitalWrite(13, HIGH);
  Point p = ts.getPoint();
  digitalWrite(13, LOW);


  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  
  //if (p.z > ts.pressureThreshhold) {
  //   Serial.print("X = "); Serial.print(p.x);
  //   Serial.print("\tY = "); Serial.print(p.y);
  //   Serial.print("\tPressure = "); Serial.println(p.z);
  //}
  //if (1 == 2) {
  if (p.z > ts.pressureThreshhold) {
    p.y = map(p.y, TS_MINY, TS_MAXY, 0, tft.height());
    
    if (p.y < tft.TFTHEIGHT/4) {
      watering_disabled = !watering_disabled;
      Serial.println("Disable watering pressed");
      drawScreen();
      //delay(2000);
    } else if (p.y < 2*tft.TFTHEIGHT/4) {
      increaseWateringLevel();
      Serial.println("Watering level pressed");
      drawScreen();
      //delay(2000);
    } else if (p.y < 3*tft.TFTHEIGHT/4) {
      Serial.println("Manual watering pressed");
      water();
      drawScreen();
      //delay(2000);
    } 
  } else {
    //Serial.println("No pushing");
    if (update_counter == 0)
    {
      Serial.println("Updating data and drawing screen.");
      //Read sensor data
      float soil_sensor = analogRead(SOIL_SENSOR_PIN);
      soilValues[counter] = int(soil_sensor);
      String soil_sensor_value = String(soil_sensor);  
    
      soil_sensor_value.toCharArray(soilSensorPrintout, 4);

      //Water
      if (soil_sensor < 500)
      {
        water();
      }
      
      drawScreen();
      //delay(2000);
      update_counter++;
    } else {
      Serial.print("Update ");
      Serial.print(update_counter);
      Serial.println(".");
      update_counter++;
      if (update_counter > 100) {
        update_counter = 0;
      }
      //delay(20);
    }
  }
}

void increaseWateringLevel()
{
  watering_level++;
  if (watering_level > 5)
  {
    watering_level = 1;
  }
}

void water()
{
  println("Watering...");
  digitalWrite(PUMP_PWM_PIN, LOW);
  delay(1000);
  analogWrite(PUMP_PWM_PIN, 255-PWM_FAST); // PWM speed = fast
  delay(watering_level*1000);
  digitalWrite(PUMP_PWM_PIN, LOW);
}

void drawScreen()
{
  //Draw to screen
  tft.fillScreen(BLACK);
  
  tft.drawRect(1, 1, tft.TFTWIDTH-2, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(6, 3, "Soil moisture level:", CYAN);
  tft.drawString(130, 3, soilSensorPrintout, WHITE);

  
  float previous = soilValues[0];
  for (int i = 1; i < VALUE_COUNT; i++)
  {
    //tft.drawLine((i-1)*(tft.TFTWIDTH/VALUE_COUNT), tft.TFTHEIGHT/4-(previous/700)*(tft.TFTHEIGHT/4),i*(tft.TFTWIDTH/VALUE_COUNT), tft.TFTHEIGHT/4-(soilValues[i]/700)*(tft.TFTHEIGHT/4));
    //previous = soilValues[i];
  }
  tft.drawRect(1, tft.TFTHEIGHT/4, tft.TFTWIDTH-2, tft.TFTHEIGHT/4, YELLOW);
  tft.fillRect(0.5*button_padding, tft.TFTHEIGHT/4+0.5*button_padding, tft.TFTWIDTH-button_padding-3, tft.TFTHEIGHT/4-button_padding, GREEN);
  tft.drawString(text_left_padding, tft.TFTHEIGHT/4+35, "Water now", BLACK);
  
  tft.drawRect(1, 2*tft.TFTHEIGHT/4, tft.TFTWIDTH-2, tft.TFTHEIGHT/4, YELLOW);
  tft.drawString(6, tft.TFTHEIGHT/2+3, "Watering level:", CYAN);
  int block_size = (tft.TFTWIDTH-button_padding) / 5;
  for (int i = 0; i < watering_level; i++)
  {
    tft.fillRect(0.5*button_padding+i*block_size, 2*tft.TFTHEIGHT/4+0.5*button_padding+10, block_size-5, tft.TFTHEIGHT/4-button_padding-10, BLUE);
  }
  
  tft.drawRect(1, 3*tft.TFTHEIGHT/4, tft.TFTWIDTH-2, tft.TFTHEIGHT/4, YELLOW);
  
  if (watering_disabled)
  {
    tft.fillRect(0.5*button_padding, 3*tft.TFTHEIGHT/4+0.5*button_padding, tft.TFTWIDTH-button_padding-3, tft.TFTHEIGHT/4-button_padding, GREEN);
    tft.drawString(text_left_padding, 3*tft.TFTHEIGHT/4+35, "Enable watering", WHITE);
  } else {
    tft.fillRect(0.5*button_padding, 3*tft.TFTHEIGHT/4+0.5*button_padding, tft.TFTWIDTH-button_padding-3, tft.TFTHEIGHT/4-button_padding, RED);
    tft.drawString(text_left_padding, 3*tft.TFTHEIGHT/4+35, "Disable watering", WHITE);
  }
  
}

