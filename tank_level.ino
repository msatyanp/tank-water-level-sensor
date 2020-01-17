
#include "DHTesp.h"           //Declarate DHT Library
#include <SimpleTimer.h>
SimpleTimer timer;

#include <BlynkSimpleEsp8266.h>
#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space

#define RelayPin 5 // D1(gpio 5)
#define BUTTON 4 //D2(gpio 4)
#define buzzerPin 15 //D8(gpio 15)
#define WaterFullLed 12 //D6 (GPIO 12)

#define DHTpin 13    //D7 (GPIO 13)
DHTesp dht;
int mq135 = A0; // smoke sensor is connected with the analog pin A0
int data = 0;
//Let's say you have your push button on pin 4
int switchState = 0; // actual read value from pin4
int oldSwitchState = 0; // last read value from pin4
int RelayON = 0; // is the switch on = 1 or off = 0

int virtualpin4 = 0; // virtual pin for 4 feed

int TANK1 = 14; //D5(gpio14)
int TANK1flowState = 0;
int buzzerstate = 0;
bool buzzerState = LOW;

int TANK2 = 16; //D0(gpio16)
int TANK2flowState = 0;
int buzzerstateTank2 = 0;

WidgetLED V11TANK1LED(V11);
WidgetLED V12TANK2LED(V12);

/* Bylnk key credentials */
char auth[] = "<BLINK AUTH KEY FROM BLINK APP>";

/* WiFi credentials */
char ssid[] = "<WIFI SSID>";
char pass[] = "<WIFI PASSWORD>";


void setup() {
 Serial.begin(115200);
 pinMode(BUTTON, INPUT); // push button
 pinMode(WaterFullLed, OUTPUT); // Set buzzer - D6
 pinMode(buzzerPin, OUTPUT); // Set buzzer - D8
 pinMode(RelayPin, OUTPUT); // anything you want to control using a switch e.g. a RelayPin
 pinMode(TANK1, INPUT);
 pinMode(TANK2, INPUT);

  //set default value
 digitalWrite(buzzerPin, LOW); // set buzzer off
 digitalWrite(RelayPin, HIGH); // set relay off
 digitalWrite(WaterFullLed,LOW); // set water led indicate off
 V11TANK1LED.off();
 V12TANK2LED.off();


 WiFi.begin(ssid, pass);     //Connect to your WiFi router
 Serial.println("");
 Blynk.begin(auth, ssid, pass);
   // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 Serial.println(WiFi.localIP());  //IP address assigned to your ESP
 
 
 dht.setup(DHTpin, DHTesp::DHT11);
 timer.setInterval(10L, getSwitchData);
 timer.setInterval(10L, getSmokeData);
 timer.setInterval(2000L, getTemperatureData);
}

void loop()
{
  Blynk.run();
  timer.run();   
}//end of void loop



/***************************************************
 * Getting switch /flow data
 **************************************************/
void getSwitchData(){
   switchState = digitalRead(BUTTON); // read the pushButton State
   TANK1flowState = digitalRead(TANK1); // read the flowButton State
   TANK2flowState = digitalRead(TANK2); // read the flowButton State
    
   if (switchState != oldSwitchState) // catch change
   {
     oldSwitchState = switchState;
     if (switchState == HIGH  && TANK1flowState == LOW)
       {
       // toggle
       RelayON = !RelayON;
       }
   }
  if (TANK1flowState == HIGH && buzzerstate == 0) {
      buzzerstate = millis();
      //Serial.println("Reset Buzzer");
  }


 if (TANK1flowState == HIGH)
   {
   // toggle
   switchState = LOW;
   digitalWrite(WaterFullLed,HIGH);
   Serial.println("Water TANK FULL LED ON");
   V11TANK1LED.on();

   RelayON = 0;
      //buzzer activation time in ms 15 secc(15000L)
       if(millis() - buzzerstate < 15000L) {
        Serial.println("Buzzer on");
        digitalWrite(buzzerPin,HIGH);
        delay(100);
        digitalWrite(buzzerPin,LOW);
        delay(100);
        }
   }else{
        digitalWrite(buzzerPin,LOW);
        digitalWrite(WaterFullLed,LOW);
        Serial.println(" LED OFFFFF");
        V11TANK1LED.off();
        buzzerstate = 0;
      }


   if(RelayON)
   {
      digitalWrite(RelayPin, LOW); // set the RelayPin on
      //Serial.println("WaterTANK ON");
      Blynk.email("maharjan.satya@gmail.com", "Subject:  Water PUMP ON", "Tank1 Water Pump is on in your home");
      Blynk.virtualWrite(V4, RelayON);
   } else {
      digitalWrite(RelayPin, HIGH); // set the RelayPin off
      Blynk.virtualWrite(V4, RelayON);
   }

/*  Tank 2 flow check*/
if (TANK2flowState == HIGH && buzzerstateTank2 == 0) {
      buzzerstateTank2 = millis();
  }
if (TANK2flowState == HIGH)
   {
   Serial.println("TANK 2 Water FULL");
   digitalWrite(WaterFullLed,HIGH);
   V12TANK2LED.on();
      //buzzer activation time in ms 15 secc(15000L)
       if(millis() - buzzerstateTank2 < 15000L) {
        digitalWrite(buzzerPin,HIGH);
        delay(100);
        digitalWrite(buzzerPin,LOW);
        delay(100);
        }

    } else  {
        digitalWrite(buzzerPin,LOW);
        digitalWrite(WaterFullLed,LOW);
        V12TANK2LED.off();
        Serial.println("TANK2 OFF");
        buzzerstateTank2 = 0;
    }

/* END of flow check TANK 2*/
  }

/***************************************************
 * Getting Temperature data
 **************************************************/
void getTemperatureData()
{
  float h = dht.getHumidity();                       //Read humidity and put it in h variable
  float t = dht.getTemperature();                    //Read temperature and put it in t variable
  Blynk.virtualWrite(V5, t);                          //Send t value to blynk in V5 virtual pin
  Blynk.virtualWrite(V6, h);                          //Send h value to blynk in V6 virtual pin

  if (isnan(h) ||isnan(t))  {
    Serial.println("Cant read from DHT11");
  }else{
    Serial.println(h);
    Serial.println(t);
  }

  if (t>40) {
      digitalWrite(buzzerPin,HIGH);
      delay(100);
      digitalWrite(buzzerPin,LOW);
      delay(100);
      Blynk.notify("The Temperature is too high");      //Blynk will send notify if t>40
      Blynk.email("maharjan.satya@gmail.com", "Subject:  Alert Temperature High", "Temperatue is Over 40 C");
    }   else  {
        digitalWrite(buzzerPin,LOW);
    }

}
/*    End of DHT11    */
/***************************************************
 * Getting Smoke data
 **************************************************/
void getSmokeData()
{
  data = analogRead(mq135);
  Serial.println(data);
  Blynk.virtualWrite(V2, data); //virtual pin V3

 if (data > 400)
   {   
     Blynk.notify("Smoke Detected!!!");   
     Blynk.email("maharjan.satya@gmail.com", "Subject:  Alert Smoke detected", "Smoke valyue  is Over 400 C"); 
      digitalWrite(buzzerPin,HIGH);
      delay(100);
      digitalWrite(buzzerPin,LOW);
      delay(100);
    } else  {
        digitalWrite(buzzerPin,LOW);
    }

}
/*    End of MQ 2 smoke sensor    */

/***************************************************
 * Rsync botton from blynk app
 **************************************************/
BLYNK_WRITE(V4) //Button Widget is writing to pin V4
{
  int virtualpin4 = param.asInt();
     switchState = virtualpin4;
     Serial.println(switchState);
     if (switchState == HIGH)
       {
        // toggle
        RelayON = !RelayON;
       }else{
        RelayON = !RelayON;
        }
     
     if (RelayON)
     {
     // Serial.println("Relay High");
      digitalWrite(RelayPin, HIGH); // set the RelayButtonOnOFF on
     }else {
      //Serial.println("Relay Low");
      digitalWrite(RelayPin, LOW); // set the RelayButtonOnOFF off
     }   
}
/*    End of bylnk button sync    */
