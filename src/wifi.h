#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>

#define INT_LED LED_BUILTIN // пин встроенного светодиода

#define WIFI_SSID "Home_Network"
#define WIFI_PASS "5VjK-8ePo-aysa-mkyr-567-bhd"

void connectWiFi();
void printWiFiStatus();