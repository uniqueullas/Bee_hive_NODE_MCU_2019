//Bee_hive_28072020.ino
//final 06082020 @5:55pm
//https://learn.sparkfun.com/tutorials/bmp180-barometric-pressure-sensor-hookup-/all
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define ALTITUDE 1090.0 // Altitude of SparkFun's HQ in Boulder, CO. in meters
#include <SFE_BMP180.h>
SFE_BMP180 pressure;
#include "HX711.h"
const int LOADCELL_DOUT_PIN = 13;
const int LOADCELL_SCK_PIN = 15;
HX711 scale;

#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11   // DHT 11
#include <Arduino.h>
DHT dht(DHTPIN, DHTTYPE);

#include <Wire.h>
char auth[] = "zj0-ZLPzY8AMpaU5sDFU0fr3LQARTA1g"; //jayanth_alvas
char ssid[] = "new1";
char pass[] = "123456789";

char statuss;
double T, P, p0, a;
int status;
int t1;
int h1;
int p1;
int c1;
int w1;
int t2;
int h2;
int p2;
int c2;
int w2;
int mail;

void sp() {
  Serial.println("Server Board");

  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE, 0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE * 3.28084, 0);
  Serial.println(" feet");


  Serial.print(String("t1: ") + T);
  Serial.println(" degC");
  Serial.print(String("p1: ") + P);
  Serial.println(" mb");
  Serial.print(String("h1: ") + h1);
  Serial.println(" % ");
  Serial.print(String("w1: ") + w1);
  Serial.println(" gms ");
  Serial.println("Clint Board");
  Serial.print(String("t2: ") + t2);
  Serial.println(" degC");
  Serial.print(String("p2: ") + p2);
  Serial.println(" Pa");
  Serial.print(String("h2: ") + h2);
  Serial.println(" % ");
  Serial.print(String("w2: ") + w2);
  Serial.println(" gms ");
  Serial.println("------------------------------------");
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  Blynk.config(auth);  // in place of Blynk.begin(auth, ssid, pass);
  dht.begin();

  Serial.println("Initializing the scale");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight (not set yet)

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // print the average of 5 readings from the ADC minus tare weight (not set) divided
  // by the SCALE parameter (not set yet)

  scale.set_scale(428.f);    // 2280.f this value is obtained by calibrating the scale with known weights; see the README for details
  scale.tare();               // reset the scale to 0

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // print a raw reading from the ADC

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));       // print the average of 20 readings from the ADC

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // print the average of 5 readings from the ADC minus the tare weight, set with tare()

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // print the average of 5 readings from the ADC minus tare weight, divided
  // by the SCALE parameter set with set_scale

  Serial.println("Readings:");
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail\n\n");
    while (1); // Pause forever.
  }

}

void loop()
{
  sp();
  c1 = analogRead(A0);
  Blynk.virtualWrite(0, c1); //audio v0

  w1 = scale.get_units(), 0;

  Blynk.virtualWrite(10, w1); //weight v10
  scale.power_down();
  scale.power_up();

  if (Serial.available() > 0) {
    String input;
    input = Serial.readStringUntil('\n');
    if (input[0] == 'c') {
      WiFi.begin(ssid, pass);
      Serial.println("connecting...");
    }
    else if (input[0] == 'd') {
      WiFi.disconnect();
      Serial.println("disconnecting...");
    }
  }
  if (WiFi.status() == WL_IDLE_STATUS) { // when not connected to a network, but powered on
    Serial.println("WL_IDLE_STATUS");
  }
  else if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WL_CONNECTED ");
    Serial.println(WiFi.localIP());
  }
  else if (WiFi.status() == WL_NO_SSID_AVAIL) {
    Serial.println("WL_NO_SSID_AVAIL");
  }
  else if (WiFi.status() == WL_CONNECT_FAILED) {
    Serial.println("WL_CONNECT_FAILED");
  }
  else if (WiFi.status() == WL_CONNECTION_LOST) {
    Serial.println("WL_CONNECTION_LOST");
  }
  else if (WiFi.status() == WL_DISCONNECTED) {
    Serial.println("WL_DISCONNECTED");
  }
  else {
    digitalWrite(BUILTIN_LED, HIGH);
    Serial.print("unknown status: ");
    Serial.println(WiFi.status());
  }



  statuss = pressure.startTemperature();
  if (statuss != 0)
  {
    delay(statuss);
    statuss = pressure.getTemperature(T);
    if (statuss != 0)
    {
      Serial.print("temperature: ");
      Serial.println(T, 2);                                                                                   //T temprature
      Serial.print((9.0 / 5.0)*T + 32.0, 2);
      Serial.println(" deg F");
      statuss = pressure.startPressure(3);
      if (statuss != 0)
      {
        delay(statuss);
        statuss = pressure.getPressure(P, T);
        if (statuss != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P, 2);                                                                               //P pressure
          Serial.print(" mb, ");
          Serial.print(P * 0.0295333727, 2);
          Serial.println(" inHg");
          p0 = pressure.sealevel(P, ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0, 2);                                                                             //p0 sea-level
          Serial.print(" mb, ");
          Serial.print(p0 * 0.0295333727, 2);
          Serial.println(" inHg");
          a = pressure.altitude(P, p0);
          Serial.print("computed altitude: ");
          Serial.print(a, 0);                                                                               
          Serial.print(" meters, ");
          Serial.print(a * 3.28084, 0);                                                                   //a altitude
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");


  Blynk.virtualWrite(1, T); //tempature V1
  Blynk.virtualWrite(2, P); //pressure v2
  Blynk.virtualWrite(15, p0); 
  Blynk.virtualWrite(16, a); 

  h1 = dht.readHumidity();
  Blynk.virtualWrite(3, h1); //humidity v3

  Blynk.run();
  BLYNK_WRITE();

  if (mail != 0) {
    String body = String("Mail button pressed \r\nTemprature1:-") + T +
                  "\r\nHumidity1:-" + h1 +
                  "\r\npreausure1:-" + P +
                  "\r\nrelative (sea-level) pressure:-" + p0 +
                  "\r\ncomputed altitude:-" + a +
                  
                  "\r\nweight1:-" + w1 +
                  "\r\nTemprature2:-" + t2 +
                  "\r\nHumidity2:-" + h2 +
                  "\r\npreausure2:-" + p2 +
                  "\r\nweight2:-" + w2 +
                  "Done";
    Serial.println(String("Mail Sent...! to: jnandu3@gmail.com" ) + body);
    Blynk.email("jnandu3@gmail.com", "Report : Bee Hive Monitorng System", body);
  }
}

BLYNK_WRITE(V9) {
  mail = param.asInt(); // button for mail v9
}
BLYNK_WRITE(V20) {
  t2 = param.asInt(); // button for mail v9
}
BLYNK_WRITE(V21) {
  h2 = param.asInt(); // button for mail v9
}
BLYNK_WRITE(V22) {
  p2 = param.asInt(); // button for mail v9
}
BLYNK_WRITE(V23) {
  w2 = param.asInt(); // button for mail v9
}
