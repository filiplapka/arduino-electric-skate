//Author: Filip Lapka
#include <SoftwareSerial.h>   // librairie pour creer une nouvelle connexion serie max 9600 baud
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <stdio.h>
#include <string.h>
#define PIN_LED 7
#define Hall_Sensor_D 2
#define Perimeter_in_km 0.0003
#define MotorPin 5

Servo Motor1;
int Speed = 0;
int hall_sensor_read=0;
int number_turns = 0;
int new_turn = 1;
float current_speed;
float last_read_time;
int display_counter = 3;
int speed_set_to_zero = 0;
float distance = 0;
int speed_of_car = 0;
int val = 0;

LiquidCrystal_I2C lcd(0x27, 16, 2);
int number_of_blinks = 0;

SoftwareSerial BTSerial(10, 11); // RX | TX  = > BT-TX=10 BT-RX=11

void setup()
{
  Serial.begin(9600);
  Serial.println("Enter a command:");
  BTSerial.begin(9600);  // HC-05 9600 baud 

  pinMode(PIN_LED, OUTPUT);
  pinMode(Hall_Sensor_D, INPUT);
  last_read_time = millis();

  Motor1.attach(MotorPin);
  Motor1.writeMicroseconds(900);

     
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("0 km/h");  
}

void loop()
{

 //Motor1.writeMicroseconds(1100);
  //delay(2000);
  //Motor1.writeMicroseconds(1500);  
  //compute_speed();
  read_bluetooth_drive_skate();
  
}

void read_bluetooth_drive_skate() {
  while (BTSerial.available() == 0);

  char val = BTSerial.read();
  Serial.println(val);
  
  if(val == '0'){
    speed_of_car = 0;
  } else if(val == '1'){
    speed_of_car = 10;
  }
  else if(val == '2'){
    speed_of_car = 20;
  }
  else if(val == '3'){
    speed_of_car = 30;
  }
  else if(val == '4'){
    speed_of_car = 40;
  }
  else if(val == '5'){
    speed_of_car = 50;
  }
  else if(val == '6'){
    speed_of_car = 60;
  }
  else if(val == '7'){
    speed_of_car = 70;
  }
  else if(val == '8'){
    speed_of_car = 80;
  }
  else if(val == '9'){
    speed_of_car = 90;
  }
  else if(val == 'q'){
    speed_of_car = 100;
  }
  else if(val == 'F'){
    Serial.print("Going forward at ");
    Serial.println(speed_of_car);
    Speed = map(speed_of_car, 0, 100, 30, 140);
    Motor1.write(Speed);  
  } 
  else if(val == 'B'){
    Serial.print("Going backwards at");
    Serial.println(speed_of_car);
  } else if(val == 'S'){
    Serial.print("Going backwards at");
    Motor1.writeMicroseconds(900);
    Serial.println(speed_of_car);
  }
}

void read_bluetooth_display_led() {
  String message;
  number_of_blinks = 0;
    while (BTSerial.available()){
      message = BTSerial.readString();
      Serial.println(message);
    }
    while (Serial.available()){
      message = Serial.readString();
      BTSerial.println(message);
    }
    if(message == "on\r\n"){
      digitalWrite(PIN_LED,HIGH);
    }// else if message off
    else if(message == "off\r\n"){
      digitalWrite(PIN_LED,LOW);
    }
    else if(message == "blink\r\n"){
      while(number_of_blinks < 5){
        digitalWrite(PIN_LED, HIGH);
        delay(1000);
        digitalWrite(PIN_LED, LOW);
        delay(1000);
        number_of_blinks += 1;
      }
    }
}

void compute_speed() {
  float last_turn_time;
  hall_sensor_read = digitalRead(Hall_Sensor_D);
  
  if(millis() - last_read_time > 70 && hall_sensor_read){ //100 milisec debouncing 
    lcd.setCursor(0,0),
    number_turns += 1;
    Serial.println(number_turns);
    distance += Perimeter_in_km;
    lcd.setCursor(0,1);
    lcd.print(distance);
    lcd.print(" km");
    last_turn_time = millis() - last_read_time;
    float current_speed = compute_current_speed_in_km_h(last_turn_time);
    Serial.println(current_speed);
    last_read_time = millis();
    display_counter--;
    speed_set_to_zero = 0;
    display_speed(current_speed);
  } 
  else if(millis() - last_read_time > 5000 && speed_set_to_zero == 0)  {
    lcd.setCursor(0,0);
    lcd.print("             ");
    lcd.setCursor(0,0);
    lcd.print("0 km/h");
    speed_set_to_zero = 1;
    } 
}


float compute_current_speed_in_km_h(float last_turn_time) {
  
  float last_turn_time_in_h = last_turn_time/3600000;
  current_speed = Perimeter_in_km / last_turn_time_in_h;

  return (current_speed);
}


void display_speed(float speed) {
  if (display_counter == 0) {
    lcd.setCursor(0,0);
    lcd.print("             ");
    lcd.setCursor(0,0);
    lcd.print(speed);
    lcd.print(" km/h");
    display_counter = 3;
  }
}
