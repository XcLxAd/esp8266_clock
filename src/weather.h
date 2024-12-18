#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <tinyxml2.h>

using namespace tinyxml2;

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult)               \
  if ((a_eResult) == nullptr)                   \
  {                                             \
    Serial.println("No weather data in block"); \
  }                                             \
  else                                          \
    Serial.println(" OK!");
#endif

class Weather
{
private:
  String regionID;
  void parseXML(String);

public:
  String weatherCurrent;
  String weatherForecast;
  Weather(String);
  void getWeatherData();
};