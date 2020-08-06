#include "DHT.h"
#define DHTPIN 14
#define DHTTYPE DHT11
#include <Arduino.h>
#include <SFE_BMP180.h>
#include <Wire.h>
SFE_BMP180 pressure;
#define ALTITUDE 1090.0
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
WidgetBridge bridge1(V1);
DHT dht(DHTPIN, DHTTYPE);
char auth[] = "PnLrwQESnDQQGPGQ2LdzGug6OSWk2wGy";//clint id board1
char ssid[] = "new1";
char pass[] = "123456789";
BLYNK_CONNECTED() {
  bridge1.setAuthToken("zj0-ZLPzY8AMpaU5sDFU0fr3LQARTA1g");// server id
}
void setup() {
  Serial.begin(9600);
  Serial.println("Bee Hive Monitoring Syatem : Board_3");
  Serial.println("Board_2 id : PnLrwQESnDQQGPGQ2LdzGug6OSWk2wGy");
  Serial.println("Board_1 id : zj0-ZLPzY8AMpaU5sDFU0fr3LQARTA1g");
  dht.begin();
  Wire.begin();
  WiFi.begin(ssid, pass);
  Blynk.config(auth);
  while (!Serial);
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while (1); // Pause forever.
  }
}
void loop() {
  float h3 = dht.readHumidity();
  Serial.print(String("humidity: ") + h3);
  Blynk.virtualWrite(21, h3);
  bridge1.virtualWrite(V21, h3);

  char status;
  double T, P, p0, a;

  Serial.println();
  Serial.print("provided altitude: ");
  Serial.print(ALTITUDE, 0);
  Serial.print(" meters, ");
  Serial.print(ALTITUDE * 3.28084, 0);
  Serial.println(" feet");


  status = pressure.startTemperature();
  if (status != 0)
  {
    // Wait for the measurement to complete:
    delay(status);

    status = pressure.getTemperature(T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("temperature: ");
      Serial.print(T, 2);
      Serial.print(" deg C, ");
      Serial.print((9.0 / 5.0)*T + 32.0, 2);
      Serial.println(" deg F");

      Blynk.virtualWrite(20, T);
      bridge1.virtualWrite(V20, T);

      status = pressure.startPressure(3);
      if (status != 0)
      {
        delay(status);

        status = pressure.getPressure(P, T);
        if (status != 0)
        {
          // Print out the measurement:
          Serial.print("absolute pressure: ");
          Serial.print(P, 2);
          Serial.print(" mb, ");
          Blynk.virtualWrite(22, P);
          bridge1.virtualWrite(V22, P);
          Serial.print(P * 0.0295333727, 2);
          Serial.println(" inHg");

          p0 = pressure.sealevel(P, ALTITUDE); // we're at 1655 meters (Boulder, CO)
          Serial.print("relative (sea-level) pressure: ");
          Serial.print(p0, 2);
          Serial.print(" mb, ");
          Serial.print(p0 * 0.0295333727, 2);
          Serial.println(" inHg");

          a = pressure.altitude(P, p0);
          Serial.print("computed altitude: ");
          Serial.print(a, 0);
          Serial.print(" meters, ");
          Serial.print(a * 3.28084, 0);
          Serial.println(" feet");
        }
        else Serial.println("error retrieving pressure measurement\n");
      }
      else Serial.println("error starting pressure measurement\n");
    }
    else Serial.println("error retrieving temperature measurement\n");
  }
  else Serial.println("error starting temperature measurement\n");
  Blynk.run();
  delay(1000);
}
