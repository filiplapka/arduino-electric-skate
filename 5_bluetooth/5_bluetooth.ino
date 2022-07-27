#include <SoftwareSerial.h>   // librairie pour creer une nouvelle connexion serie max 9600 baud
#define PIN_LED 7
int number_of_blinks = 0;

SoftwareSerial BTSerial(10, 11); // RX | TX  = > BT-TX=10 BT-RX=11

void setup()
{
  Serial.begin(9600);
  Serial.println("Enter a command:");
  BTSerial.begin(9600);  // HC-05 9600 baud 

  pinMode(PIN_LED, OUTPUT);
}

void loop()
{
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
