//Bee_hive_28072020.ino
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <BMP180MI.h>
#define I2C_ADDRESS 0x77
BMP180I2C bmp180(I2C_ADDRESS);

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

int status;

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

  while (!Serial);
  Wire.begin();
  //begin() initializes the interface, checks the sensor ID and reads the calibration parameters.
  if (!bmp180.begin())
  {
    Serial.println("begin() failed. check your BMP180 Interface and I2C Address.");
    while (1);
  }
  //reset sensor to default parameters.
  bmp180.resetToDefaults();
  //enable ultra high resolution mode for pressure measurements
  bmp180.setSamplingMode(BMP180MI::MODE_UHR);
}

int t1;
int h1;
int p1;
int c1;
int w1;
int mail;

void loop()
{
  sp();
  c1 = analogRead(A0);
  Blynk.virtualWrite(0, c1); //audio v0

  w1 = scale.get_units(), 0;
  Serial.print(String("w1: ") + w1);
  Serial.println(" gms ");
  Blynk.virtualWrite(10, w1); //weight v10
  scale.power_down();  
  delay(5000);
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
    // lcd.setCursor(-4, 3);
    // lcd.print("WL_IDLE_STATUS");
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


  if (!bmp180.measureTemperature())
  {
    Serial.println("could not start temperature measurement, is a measurement already running?");
    return;
  }

  //wait for the measurement to finish. proceed as soon as hasValue() returned true.
  do
  {
    delay(100);
  } while (!bmp180.hasValue());

  t1 = bmp180.getTemperature();
  Blynk.virtualWrite(1, t1); //tempature V1

  if (!bmp180.measurePressure())
  {
    Serial.println("could not start perssure measurement, is a measurement already running?");
    return;
  }
  do
  {
    delay(100);
  } while (!bmp180.hasValue());

  p1 = bmp180.getPressure();
  Blynk.virtualWrite(2, p1); //pressure v2

  h1 = dht.readHumidity();
  Blynk.virtualWrite(3, h1); //humidity v3

  Blynk.run();
  BLYNK_WRITE();

  if (mail != 0) {
    String body = String("Mail button pressed \r\nTemprature1") + t1 +
                  "\r\nHumidity1:" + h1 +
                  "\r\npreausure1:" + p1 +
                  "Done";
    Serial.println(String("Mail Sent...!" ) + body);
    Blynk.email("ullas6558@gmail.com", "Report", body);
  }
}

BLYNK_WRITE(V9) {
  mail = param.asInt(); // button for mail v9
}

void sp() {
  Serial.print(String("t1: ") + t1);
  Serial.println(" degC");
  Serial.print(String("p1: ") + p1);
  Serial.println(" Pa");
  Serial.print(String("h1: ") + h1);
  Serial.println(" % ");
  Serial.print(String("w1: ") + w1);
  Serial.println(" gms ");
}
