//Author: Filip Lapka
#include <SoftwareSerial.h>   // librairie pour creer une nouvelle connexion serie max 9600 baud
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <stdio.h>
#include <string.h>

#define DIGITAL_HALL_SENSOR_PIN 2
#define PERIMETER_OF_WHEEL_IN_KM 0.0003
#define DIGITAL_MOTOR_PIN 5

//Configuration of skate engine
Servo skate_motor;

//Configuraion of LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);

//Configuration of bluetooth
SoftwareSerial BTSerial(10, 11); // RX | TX  = > BT-TX=10 BT-RX=11

int computed_skate_requested_speed = 30;
int hall_sensor_read=0;
int number_turns = 0;
float current_speed;
float hall_sensor_last_read_time;
int display_counter = 3;
int speed_set_to_zero = 0;
float distance = 0;
int speed_requested_by_bluetooth = 0;
int delay_time_when_computing_speed = 250;


//This method is called when Arduino is started
void setup()
{
  //Initialize display on a computer screen (Serial monitor)
  Serial.begin(9600);

  //Initialize connection with a bluetooth 
  BTSerial.begin(9600);

  //Connect to a digital pin reading data from hall sensor
  pinMode(DIGITAL_HALL_SENSOR_PIN, INPUT);
  
  hall_sensor_last_read_time = millis();

  //Connect to engine
  skate_motor.attach(DIGITAL_MOTOR_PIN);
  skate_motor.write(computed_skate_requested_speed);
     
  // initialize the LCD
  lcd.begin();

  // Turn on the blacklight and print a message.
  lcd.backlight();
  lcd.print("0 km/h");  
}

void loop()
{
  compute_delay_time_for_speed_computation();
  compute_speed();
    
  while (BTSerial.available()) {    
    read_bluetooth_drive_skate();
  }
}

void Speed_and_direction(int requestedSpeed){
    computed_skate_requested_speed = map(requestedSpeed, 0, 100, 30, 140);
    Serial.print("Going forward at ");
    Serial.println(computed_skate_requested_speed);    
    skate_motor.write(computed_skate_requested_speed);    
}
    
void read_bluetooth_drive_skate() {
  char bluetooth_request = BTSerial.read();
  Serial.println(bluetooth_request);
  
  if(bluetooth_request == '0'){
    speed_requested_by_bluetooth = 0;
    Speed_and_direction(speed_requested_by_bluetooth);
  } 
  else if(bluetooth_request == '1'){
    speed_requested_by_bluetooth = 10;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
  else if(bluetooth_request == '2'){
    speed_requested_by_bluetooth = 20;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
  else if(bluetooth_request == '3'){
    speed_requested_by_bluetooth = 30;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
  else if(bluetooth_request == '4'){
    speed_requested_by_bluetooth = 40;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
  else if(bluetooth_request == '5'){
    speed_requested_by_bluetooth = 50;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
  else if(bluetooth_request == '6'){
    speed_requested_by_bluetooth = 60;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
  else if(bluetooth_request == '7'){
    speed_requested_by_bluetooth = 70;
    Speed_and_direction(speed_requested_by_bluetooth); 
  }
  else if(bluetooth_request == '8'){
    speed_requested_by_bluetooth = 80;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
  else if(bluetooth_request == '9'){
    speed_requested_by_bluetooth = 90;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
  else if(bluetooth_request == 'q'){
    speed_requested_by_bluetooth = 100;
    Speed_and_direction(speed_requested_by_bluetooth);
  }
}

void compute_speed() {
  float last_turn_time;
  hall_sensor_read = digitalRead(DIGITAL_HALL_SENSOR_PIN);
  
  if(millis() - hall_sensor_last_read_time > delay_time_when_computing_speed && hall_sensor_read){ //Wait 70 miliseconds before reading sensor again
     
    lcd.setCursor(0,0),
    number_turns += 1;
    Serial.println(number_turns);
    distance += PERIMETER_OF_WHEEL_IN_KM;
    lcd.setCursor(0,1);
    lcd.print(distance);
    lcd.print(" km");
    last_turn_time = millis() - hall_sensor_last_read_time;
    float current_speed = compute_current_speed_in_km_h(last_turn_time);
    Serial.println(current_speed);
    hall_sensor_last_read_time = millis();
    display_counter--;
    speed_set_to_zero = 0;
    display_speed(current_speed);
  } 
  else if(millis() - hall_sensor_last_read_time > 5000 && speed_set_to_zero == 0)  {
    lcd.setCursor(0,0);
    lcd.print("             ");
    lcd.setCursor(0,0);
    lcd.print("0 km/h");
    lcd.setCursor(0,1);
    lcd.print(distance);
    lcd.print(" km");    
    current_speed = 0;
    speed_set_to_zero = 1;
  }
}

void compute_delay_time_for_speed_computation() {
  if (current_speed < 2) {
    delay_time_when_computing_speed = 200;
    Serial.print("1 delay time is ");
    Serial.println( delay_time_when_computing_speed);  
  }
  else if (current_speed >= 2 && current_speed < 5) {
    delay_time_when_computing_speed = 150;
    Serial.print("2 delay time is ");
    Serial.println( delay_time_when_computing_speed); 
  }  
  else if (current_speed >= 5 && current_speed < 10) {
    delay_time_when_computing_speed = 90;
    Serial.print("2 delay time is ");
    Serial.println( delay_time_when_computing_speed); 
  }
  else if (current_speed >= 10 && current_speed < 15) {
    delay_time_when_computing_speed = 60;
    Serial.print("3 delay time is ");
    Serial.println( delay_time_when_computing_speed); 
  }
  else if (current_speed >= 15 && current_speed < 20) {
    delay_time_when_computing_speed = 40;
    Serial.print("4 delay time is ");
    Serial.println( delay_time_when_computing_speed); 
  }  
  else if (current_speed >= 20) {
    delay_time_when_computing_speed = 15;
    Serial.print("5 delay time is ");
    Serial.println( delay_time_when_computing_speed); 
  }
}


float compute_current_speed_in_km_h(float last_turn_time) {
  
  float last_turn_time_in_h = last_turn_time/3600000;
  current_speed = PERIMETER_OF_WHEEL_IN_KM / last_turn_time_in_h;

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
