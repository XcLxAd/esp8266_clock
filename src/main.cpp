using namespace std;
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
/**
 * 
 */
#include <time.h>
#include <sntp.h>
#include <TZ.h>
#define MYTZ TZ_Europe_Moscow
/**
 * 
 */
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#ifndef STASSID
#define STASSID "Home_Network"
#define STAPSK  "5VjK-8ePo-aysa-mkyr-567-bhd"
#endif
const char* ssid = STASSID;
const char* password = STAPSK;
char ipAddressChr [16];
/**
 * 
 */
#include "MD_MAX72xx.h"
#include "MD_Parola.h"
#include "6bite_rus.h"
#include "5bite_rus.h"
#define CS_PIN	    0 // подключение пина CS
#define MAX_DEVICES 4
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // Use FC-16 style hardware module
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES); // SPI hardware interface

typedef struct {
  textEffect_t effect;   // text effect to display
  uint16_t speed;    // speed multiplier of library default
} sCatalog;

sCatalog catalog[] = {
    {PA_SLICE, 10},
    {PA_MESH, 10},
    {PA_FADE, 20},
    {PA_WIPE, 50},
    {PA_WIPE_CURSOR, 40},
    {PA_OPENING, 30},
    {PA_OPENING_CURSOR, 40},
    {PA_CLOSING, 30},
    {PA_CLOSING_CURSOR, 40},
    {PA_BLINDS, 70},
    {PA_DISSOLVE, 70},
    {PA_SCROLL_UP, 50},
    {PA_SCROLL_DOWN, 50},
    {PA_SCROLL_LEFT, 50},
    {PA_SCROLL_RIGHT, 50},
    {PA_SCROLL_UP_LEFT, 70},
    {PA_SCROLL_UP_RIGHT, 70},
    {PA_SCROLL_DOWN_LEFT, 70},
    {PA_SCROLL_DOWN_RIGHT, 70},
    {PA_SCAN_HORIZ, 40},
    {PA_SCAN_VERT, 30},
    {PA_GROW_UP, 70},
    {PA_GROW_DOWN, 70},
};

/**
 * 
 */
int rnd;
int disp = 0;
int startT = 1;
int endT = 1;

String Text;
char buf[256];

String y;   // год
String mon; // месяц
String wd;  // день недели
String md;   // день месяца
String h;   // часы
String m;   // минуты
String s;   // секунды
uint8_t second;

/**
 * 
 */
void displayTime() {
  if (P.displayAnimate()) { //о том что анимация закончена функция displayAnimate() сообщает переходом в значение TRUE
    //P.setFont(_6bite_rus);
    Text.toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, 200, 0, PA_PRINT, PA_NO_EFFECT); //(Текст для вывода, выравнивание текста, скорость, задержка показа, эффект появления, эффект исчезновения)
    if (!(second % 30)) disp = 1;     
  }
}
/**
 * 
 */
void displayTimeStart() {
  if (P.displayAnimate()) { //о том что анимация закончена функция displayAnimate() сообщает переходом в значение TRUE
    rnd = random(0, ARRAY_SIZE(catalog));
    P.setFont(_6bite_rus);
    Text.toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 0, catalog[rnd].effect, PA_NO_EFFECT);
    if (!P.displayAnimate()) {
      startT = 0;
      endT = 1;
    }
    if (!(second % 30)) disp = 1;
  }
}
/**
 * 
 */
void displayTimeEnd() {
  if (P.displayAnimate()) { //о том что анимация закончена функция displayAnimate() сообщает переходом в значение TRUE
    rnd = random(0, ARRAY_SIZE(catalog));
    P.setFont(_6bite_rus);
    Text.toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 0, PA_PRINT, catalog[rnd].effect);
    if (!P.displayAnimate()) endT = 0;
   }
} 
/**
 * 
 */
void displayScroll() {
  if (P.displayAnimate()) {
    P.setFont(_5bite_rus);
    Text.toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 40); // вывод бегущей строки с
    disp = 0;
    startT = 1;
  }
}

/**
 * 
 */
void getTime() {
  time_t now = time(nullptr);
  const struct tm * timeinfo = localtime(&now);
 
  h = String (timeinfo->tm_hour/10) + String (timeinfo->tm_hour%10);
  m = String (timeinfo->tm_min/10) + String (timeinfo->tm_min%10);
  s = String (timeinfo->tm_sec/10 + String (timeinfo->tm_sec%10));
  second = timeinfo->tm_sec;
  md = String (timeinfo->tm_mday);
  y = String (timeinfo->tm_year + 1900);
  if (timeinfo->tm_mon + 1 == 1) mon = "января";
  if (timeinfo->tm_mon + 1 == 2) mon = "февраля";
  if (timeinfo->tm_mon + 1 == 3) mon = "марта";
  if (timeinfo->tm_mon + 1 == 4) mon = "апреля";
  if (timeinfo->tm_mon + 1 == 5) mon = "мая";
  if (timeinfo->tm_mon + 1 == 6) mon = "июня";
  if (timeinfo->tm_mon + 1 == 7) mon = "июля";
  if (timeinfo->tm_mon + 1 == 8) mon = "августа";
  if (timeinfo->tm_mon + 1 == 9) mon = "сентября";
  if (timeinfo->tm_mon + 1 == 10) mon = "октября";
  if (timeinfo->tm_mon + 1 == 11) mon = "ноября";
  if (timeinfo->tm_mon + 1 == 12) mon = "декабря";

  if (timeinfo->tm_wday == 1) wd = "понедельник";
  if (timeinfo->tm_wday == 2) wd = "вторник";
  if (timeinfo->tm_wday == 3) wd = "среда";
  if (timeinfo->tm_wday == 4) wd = "четверг";
  if (timeinfo->tm_wday == 5) wd = "пятница";
  if (timeinfo->tm_wday == 6) wd = "суббота";
  if (timeinfo->tm_wday == 7) wd = "воскресенье";
}

/**
 *
 */
void setup() {

  Serial.begin(115200);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
/**
 *ArduinoOTA SETUP
 */
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
/**
 *MD_Parola SETUP
 */
  P.begin();
  P.setInvert(false);
  
  

  String ipAddress = WiFi.localIP().toString(); // получаем ip адрес в виде String 
  ipAddress.toCharArray(ipAddressChr, 16); // преобразуем ip адрес из string в CharArray
 
  P.displayAnimate();
  Text = ipAddressChr;
  displayScroll();
  //P.displayScroll(ipAddressChr, PA_LEFT, PA_SCROLL_LEFT, 25); // вывод бегущей строки с ip адресом
/**
 * 
 */
  configTime(MYTZ, "time.nist.gov", "time.windows.com", "ru.pool.ntp.org");
  while(!sntp_enabled()) {
    Serial.print("-");
    delay(100);
  } 
}


void loop() {
  ArduinoOTA.handle();
  getTime();
  if (disp == 0) {     
    if (second & 1) Text = h + ":" + m;
    else Text = h + " " + m;
    if (startT) displayTimeStart();
    else displayTime();
 }
  if (disp == 1) {
    if (endT) {
      if (second & 1) Text = h + ":" + m;
      else Text = h + " " + m;
       displayTimeEnd();
    }
    else {
      Text = wd + " " + md + " " + mon + " " + y + "г";
      displayScroll();
    }
  }   
}