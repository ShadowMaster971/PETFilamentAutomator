/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/
#include "pins.hpp"
#include "leds.hpp"
#include "pins.hpp"
#include "conf.hpp"
#include "wifi.hpp"
#include "hotend.hpp"
#include "oled.hpp"
#include "stepper.hpp"
#include "server.hpp"
#include "ota.hpp"

//double Ft = 0; //filament total
//double Tt = 0; //filament total
//double filamentSession = 0; //Longueur de filament durant la session
//double durationSession = 0;   //Durée de la session

double tempLastStats;

StaticJsonDocument<128> stats;

void setup() {
  Serial.begin(115200);
  //Serial.println("Initializing");
  delay(1000);
  initBuiltInLed();
  initConf();
  initWiFi();
  initOTA();
  initHotEnd();
  initStepper();
  initOled();
  InitServer();
  tempLastStats = millis();
  
  File file = SPIFFS.open("/stats.json", "r");
  DeserializationError error = deserializeJson(stats, file);
  if (error) {

      Serial.println(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
  } else {
    Ft  = stats["Ft"]?stats["Ft"].as<double>():0.0;
    Tt  = stats["Tt"]?stats["Tt"].as<double>():0.0;
  }
  file.close();
  
}

// the loop function runs over and over again forever
void loop() {
  blinkLed();
  displayOled();
  wifiTask();
  server.handleClient();
  hotendReadTempTask();
  stepperRunTask();
  ArduinoOTA.handle();
  readConfigurationSerial();
  if ((F || !filamentSensorEnable) && status=="working" && millis() >= tempLastStats + 5000) {
    filamentSession = filamentSession + (float)Vo/2/62*5;
    Ft = Ft + (float)Vo/2/62*5; // centimetros hechos cada 5000 miliseconds
    Tt = Tt + 5;
    durationSession = durationSession + 5;
    
    File file = SPIFFS.open("/stats.json", "w");
    if (!file) {
      msg = "Failed to create file";
    }
    stats["Ft"] = Ft;
    stats["Tt"] = Tt;
    if (serializeJson(stats, file) == 0) {
      msg = "Failed to write to file";
    }
    file.close();
    
    tempLastStats = millis();
  }
}
