/******************************************************************************
 * Copyright 2018 Google
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#if defined(ARDUINO_SAMD_MKR1000) or defined(ESP32)
#define __SKIP_ESP8266__
#endif

#if defined(ESP8266)
#define __ESP8266_MQTT__
#endif

#ifdef __SKIP_ESP8266__

#include <Arduino.h>

void setup(){
  Serial.begin(115200);
}

void loop(){
  Serial.println("Hello World");
}

#endif

#ifdef __ESP8266_MQTT__
#include <CloudIoTCore.h>
#include "esp8266_mqtt.h"
#include "DHT.h"
#include <TimeLib.h>

#ifndef LED_BUILTIN
#define LED_BUILTIN 13
#endif

#define DHTTYPE DHT11   // DHT 11 
#define DHTPIN4 D4     // Digital pin connected to the DHT sensor
#define DHTPIN5 D5     // Digital pin connected to the DHT sensor

DHT dht4(DHTPIN4, DHTTYPE);
DHT dht5(DHTPIN5, DHTTYPE);

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  setupCloudIoT(); // Creates globals for MQTT
  dht4.begin();
  dht5.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  time_t t = time(nullptr);
  setTime(hour(t)+7,minute(t),second(t),day(t),month(t),year(t));
}

static unsigned long lastMillis = 0;
void loop()
{
  float humidity4 = dht4.readHumidity();
  float temperature4 = dht4.readTemperature();
  float humidity5 = dht5.readHumidity();
  float temperature5 = dht5.readTemperature();
  time_t t = now();
  
  if (!mqtt->loop())
  {
    mqtt->mqttConnect();
  }

  delay(10); // <- fixes some issues with WiFi stability

  String tahun, bulan, hari, jam, menit, detik;
  
  // TODO: Replace with your code here
  if (millis() - lastMillis > 60000)
  {
    lastMillis = millis();
    
    
    if(year(t) <10){tahun = "0"+String(year(t));} else { tahun = String(year(t));}
    if(month(t) <10){bulan = "0"+String(month(t));} else { bulan = String(month(t));}
    if(day(t) <10){hari = "0"+String(day(t));} else { hari = String(day(t));}
    if(hour(t) <10){jam = "0"+String(hour(t));} else { jam = String(hour(t));}
    if(minute(t) <10){menit = "0"+String(minute(t));} else { menit = String(minute(t));}
    if(second(t) <10){detik = "0"+String(second(t));} else { detik = String(second(t));}
    
    
    String timestamp = ",\"time\":\"" + String(tahun) + "-" + String(bulan) +"-"+ String(hari) +" "+ String(jam) +":"+ String(menit) +":"+ String(detik) + ".00\"}";
    
    String payload4 = "{\"dev1\":\"dev_pin_4\",\"Temp1\":"+String(temperature4)+",\"hum1\":"+String(humidity4);
    String payload5 = ",\"dev2\":\"dev_pin_5\",\"Temp2\":"+String(temperature5)+",\"hum2\":"+String(humidity5);

    publishTelemetry(payload4+payload5+timestamp);    
    
  }
}
#endif
