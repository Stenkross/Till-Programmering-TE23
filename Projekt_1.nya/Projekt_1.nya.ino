/*
* Name: clock and temp project
* Author: Sebatian Loeskow
* Date: 2024-10-10
* Description: This project uses a ds3231 to measure time and displays the time to an 1306 oled display,
* Further, it measures temprature with a analog temprature module and displays a mapped value to a 9g-servo-motor
*/

// Include Libraries
#include <Servo.h>
#include <RTClib.h>
#include <Wire.h>
#include "U8glib.h"

// Init constants
const int tempPin = A1;
int motor = 10;
int pie = 9;


// Init global variables
int pos = 0;
char t[32];
float temp;

// construct objects
Servo myservo;
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
RTC_DS3231 rtc;

void setup() {
  // init communication
  Serial.begin(9600);
  Wire.begin();

  // Init Hardware
  pinMode(motor,OUTPUT);
  pinMode(pie,OUTPUT);
  pinMode(A0, OUTPUT);
  myservo.attach(8);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  pinMode(tempPin, INPUT);
  u8g.setFont(u8g_font_unifont);
}

void loop() {

  oledWrite(String(getTime()), String(getTemp()) , 30, 40);

  servoWrite(getTemp());
  Serial.println(getTemp());
  delay(1000);
  dcpie();

}


/*
*This function reads time from an ds3231 module and package the time as a String
*Parameters: Void
*Returns: time in hh:mm:ss as String
*/
String getTime() {

  DateTime now = rtc.now();
  return String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

  delay(1000);
}

/*
* This function reads an analog pin connected to an analog temprature sensor and calculates the corresponding temp
*Parameters: Void
*Returns: temprature as float
*/
float getTemp() {
  float temp = 0;
  float R1 = 10000;
  float logR2, R2, T;
  float c1 = 0.001129148, c2 = 0.000234125, c3 = 0.0000000876741;

  int Vo = analogRead(tempPin);
  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  temp = (1.0 / (c1 + c2 * logR2 + c3 * logR2 * logR2 * logR2));
  temp = temp - 273.15;

  return temp;
}

/*
* This function takes a string and draws it to an oled display
*Parameters: - text: String to write to display
*Returns: void
*/
void oledWrite(String text, String text2, int x, int y) {
  u8g.firstPage();
  do {
    u8g.drawStr(x, y, text.c_str());
    u8g.drawStr(30, 20, text2.c_str());
  } while (u8g.nextPage());
}

/*
* takes a temprature value and maps it to corresppnding degree on a servo
*Parameters: - value: temprature
*Returns: void
*/
void servoWrite(float value) {
  float deg = map(value, 23, 30, 0, 180);
  pos = deg;
  delay(15);
  myservo.write(pos);
}

/*
* Controls a motor and buzzer based on temperature, if the temp gets over 24 C the motor starts to cool down the thermometer and a warning sound starts playing.
*Parameters: - value: temprature
*Returns: void
*/
void dcpie(){ 
  if (getTemp()> 24){
    digitalWrite(motor, 1);
     tone(pie, 50);
     delay(1000);
    noTone(pie);
  }
  else {
    digitalWrite(motor, 0);
  }
}