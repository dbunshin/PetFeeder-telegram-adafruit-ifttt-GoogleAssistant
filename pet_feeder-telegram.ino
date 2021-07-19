// board manager esp8266 2.7.4
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include<Servo.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h> 
#include "CTBot.h"

CTBot myBot;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "0.id.pool.ntp.org", 25200,60000);

Servo servo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define WIFI_SSID "xxxx"
#define WIFI_PASS "xxx"

#define MQTT_SERV "io.adafruit.com"
#define MQTT_PORT 1883
#define MQTT_NAME "xxx"
#define MQTT_PASS "xxxx"

//  int SERVO_PIN = D3;    // The pin which the servo is attached to
int SERVO_PIN = D4;    // The pin which the servo is attached to
int CLOSE_ANGLE = 0;  // The closing angle of the servo motor arm asli 0
int OPEN_ANGLE = 180;  // The opening angle of the servo motor arm asli 60
int  hh, mm, ss;
int feed_hour = 0;
int feed_minute = 0;

//----- token telegram bot
String ssid = "xxx";     // REPLACE mySSID WITH YOUR WIFI SSID
String pass = "xxx"; // REPLACE myPassword YOUR WIFI PASSWORD, IF ANY
String token = "xxx";

//Set up MQTT and WiFi clients
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERV, MQTT_PORT, MQTT_NAME, MQTT_PASS);

//Set up the feed you're subscribing to
Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, MQTT_NAME "/f/onoff");
boolean feed = true; // condition for alarm

void setup()
{
// initialize digital pin LED_BUILTIN as an output.
//  pinMode(LED_BUILTIN, OUTPUT);
  
  Serial.begin(9600);
  timeClient.begin();
  Wire.begin(D2, D1);

// begin error
//  lcd.begin(); 
 
// ini bisa
  lcd.begin(16,2);
  
// ini bisa
  lcd.init();

// tambahan 
  lcd.backlight();
  
  //Connect to WiFi
  Serial.print("\n\nConnecting Wifi... ");
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED)
  Serial.print(WiFi.localIP());
  {
    delay(1000);
  }
  
  //Subscribe to the onoff feed
  mqtt.subscribe(&onoff);
  servo.attach(SERVO_PIN);
  servo.write(CLOSE_ANGLE);
  Serial.println("\nServo Check");

  {
    delay(3000);
  }

//----- set the telegram connection
//////
   Serial.println("\n\nStarting Telegram Bot...");
// connect the ESP8266 to the desired access point
   myBot.wifiConnect(ssid, pass);
   myBot.setTelegramToken(token);
// check if all things are ok
  if (myBot.testConnection())
    Serial.println("\nTest Telegram Connection OK");
  else
    Serial.println("\nTest Telegram Connection NOK");

/////

}

void loop()
{
// LED indicator
//  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
//  delay(100);                       // wait for a second
//  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
//  delay(100);                       // wait for a second
  
   MQTT_connect();
   timeClient.update();
   hh = timeClient.getHours();
   mm = timeClient.getMinutes();
   ss = timeClient.getSeconds();
   
   lcd.setCursor(0,0);
   lcd.print("Time:");
    if(hh>12)
  {
    hh=hh-12;
    lcd.print(hh);
    lcd.print(":");
    lcd.print(mm);
    lcd.print(":");
    lcd.print(ss);
    lcd.println(" PM  ");
  }
  else
  {
    lcd.print(hh);
    lcd.print(":");
    lcd.print(mm);
    lcd.print(":");
    lcd.print(ss);
    lcd.println(" AM  ");   
  }
   lcd.setCursor(0,1);
   lcd.print("Feed Time:");
   lcd.print(feed_hour);
   lcd.print(':');
   lcd.print(feed_minute   );

  Adafruit_MQTT_Subscribe * subscription;
  while ((subscription = mqtt.readSubscription(5000)))
  {
    
    if (subscription == &onoff)
    {
      //Print the new value to the serial monitor
      Serial.println((char*) onoff.lastread);
     
    if (!strcmp((char*) onoff.lastread, "ON"))
      {
        
        open_door();
        delay(1500);
        close_door();
       }
      if (!strcmp((char*) onoff.lastread, "Morning"))
      {
        feed_hour = 05;
        feed_minute = 30; 
      }
      if (!strcmp((char*) onoff.lastread, "Afternoon"))
      {
        feed_hour = 03;
        feed_minute = 30; 
      }
      if (!strcmp((char*) onoff.lastread, "Evening"))
      {
        feed_hour = 07;
        feed_minute = 30; 
      }
     }
   }
   if( hh == feed_hour && mm == feed_minute &&feed==true) //Comparing the current time with the Feed time

    { 
      open_door();
      delay(1500);
      close_door();
      feed= false; 
      }
    
  
  // --- begin telegram bot
  
    // a variable to store telegram message data
  TBMessage msg;

  // if there is an incoming message...
  if (myBot.getNewMessage(msg)) {
    if (msg.text.equalsIgnoreCase("Mamam")) {                   //Perintah dari telegram ke perangkat   
      open_door();
      delay(1500);
      close_door();
      myBot.sendMessage(msg.sender.id, "Makaciiih, aku udah dikasih Mamam"); //Balasan dari perangkat ke Bot Telegram
      Serial.println("Mamam Sukses");
    }
    else if (msg.text.equalsIgnoreCase("Makan")) {              //Perintah dari telegram ke perangkat
      open_door();
      delay(1500);
      close_door();   
      myBot.sendMessage(msg.sender.id, "Kami segenap KuDa alias Kucing Dagul, Mocca - Chino - Krimpi mengucapkan, terima kasih banyak atas makanan yang diberikan ke kami hari ini, semoga Ndoro selalu dijaga kesehatannya, dilacarkan rezekinya, dan diberkahi oleh Allah SWT, Amin Amin Ya Rabbal Alamin"); //Balasan dari perangkat ke Bot Telegram
      Serial.println("Makan Sukses");
    }
    else if (msg.text.equalsIgnoreCase("Mangan")) {              //Perintah dari telegram ke perangkat
      open_door();
      delay(1500);
      close_door();   
      myBot.sendMessage(msg.sender.id, "Suwun wis di kei mangan, kulo dahar riyin ngiih Ndoro.."); //Balasan dari perangkat ke Bot Telegram
      Serial.println("Mangan ben wareg");
    }
    else if (msg.text.equalsIgnoreCase("Madang")) {              //Perintah dari telegram ke perangkat
      open_door();
      delay(1500);
      close_door();   
      myBot.sendMessage(msg.sender.id, "Suwun wis di kei madang, kulo madang riyin ngiih Ndoro.."); //Balasan dari perangkat ke Bot Telegram
      Serial.println("Madang ben wareg");
    }
    else if (msg.text.equalsIgnoreCase("Mbadok")) {              //Perintah dari telegram ke perangkat
      open_door();
      delay(2000);
      close_door();   
      myBot.sendMessage(msg.sender.id, "Suwun wis di paringi badokan gadah, kulo mbadok riyin ngiih Ndoro.."); //Balasan dari perangkat ke Bot Telegram
      Serial.println("Mbadok ben wareg");
    }
    else {                                                    // otherwise...
      // generate the message for the sender
      String reply;
      reply = (String)"Met dateng ya " + msg.sender.username + (String)". Kata kuncinya adalah Makan dan Mamam";
      myBot.sendMessage(msg.sender.id, reply);             // and send it
    }
  }
  
  /// -- end telegram bot   
    
}

void MQTT_connect() 
{
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) 
  {
    return;
  }

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) // connect will return 0 for connected
  { 
       
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) 
       {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  
}

void open_door(){
  
  servo.write(OPEN_ANGLE);   // Send the command to the servo motor to open the trap door
}

void close_door(){
  
  servo.write(CLOSE_ANGLE);   // Send te command to the servo motor to close the trap door
}
