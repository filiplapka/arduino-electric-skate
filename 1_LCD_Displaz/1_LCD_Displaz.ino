#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <stdio.h>
#include <string.h>

#define Hall_Sensor_D 2
#define Perimeter_in_km 0.0003

int hall_sensor_read=0;
int number_turns = 0;
int new_turn = 1;
float current_speed;
float last_read_time;
int display_counter = 3;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup()
{
  Serial.begin(9600);
  pinMode(Hall_Sensor_D, INPUT);
  last_read_time = millis();

    
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("0 km/h");
}

void loop(){ 
  compute_speed();
}

void compute_speed() {
  float last_turn_time;
  hall_sensor_read = digitalRead(Hall_Sensor_D);
  
  if(millis() - last_read_time > 70 && hall_sensor_read){ //100 milisec debouncing 
    number_turns += 1;
    Serial.println(number_turns);

    last_turn_time = millis() - last_read_time;
    float current_speed = compute_current_speed_in_km_h(last_turn_time);
    Serial.println(current_speed);
    last_read_time = millis();
    display_counter--;

    display_speed(current_speed);
  }
}

float compute_current_speed_in_km_h(float last_turn_time) {
  
  float last_turn_time_in_h = last_turn_time/3600000;
  current_speed = Perimeter_in_km / last_turn_time_in_h;

  return (current_speed);
}


void display_speed(float speed) {
  if (display_counter == 0) {
    lcd.clear();
    lcd.print(speed);
    lcd.print(" km/h");
    display_counter = 3;
  }
}
