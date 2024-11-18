#include <Arduino.h>
#include <ArduinoJson.h>
//#include <ESP8266HTTPClient.h>
//#include <ESP8266WiFi.h>

#include <string.h>
#include <iostream>
#include <sstream>


#include <variables.h>
#include <wifi.h> //локальная библиотека подключения к WiFi
#include <weather.h>

/**
 * подключение библиотек работы со временем
 */
#include <TZ.h>
#include <sntp.h>
#include <time.h>

/**
 * подключение библиотек работы с матрицей и эффектов
 */
#include "MD_MAX72xx.h"
#include "MD_Parola.h"

/**
 * подключение шрифтов
 */
#include "5bite_rus.h"
#include "6bite_rus.h"
/**
 * включение режима отладки
 */
// #define DEBUG

/**
 * включение ночного режима
 */
#define Night_Bbrightness

/**
 * установка временной зоны
 */
#define MYTZ TZ_Europe_Moscow

/**
 * установки индикации
 */
#define MAX_DEVICES 4                     // количество матриц
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // используются матрицы типа FC-16
/** подключение пинов Wemos D1 mini
 +----------------------+---------------+
 | DEVISES              | Wemos D1 mini |
 +----------------------+---------------+
 | MAX7219 DIN          | D7/GPIO 13    |
 | MAX7219 CS           | D3/GPIO 0     |
 | MAX7219 CLK          | D5/GPIO 14    |
 +----------------------+---------------+
 | GND - photoresistor  | A0/ADC0       |
 | +3,3V - resistor 10k | A0/ADC0       |
 +----------------------+---------------+
*/
#define MAX7219_CS_PIN 0 // D3/GPIO 0
// #define PHOTORESISTOR_PIN A0 // A0/ADC0 пин фоторезистора не используется

/**
 * настройка анимации
 */
MD_Parola P = MD_Parola(HARDWARE_TYPE, MAX7219_CS_PIN,
                        MAX_DEVICES); // выбор интерфейса SPI

struct sCatalog
{
  textEffect_t effect; // эффект
  uint16_t speed;      // скорость эффекта
};

sCatalog catalog[] = {
    {PA_SCROLL_UP, 5},
    {PA_SCROLL_DOWN, 5},
    {PA_SCROLL_LEFT, 5},
    {PA_SCROLL_RIGHT, 5},
#if ENA_SPRITE
    {PA_SPRITE, 5},
#endif
#if ENA_MISC
    {PA_SLICE, 1},
    {PA_MESH, 20},
    {PA_FADE, 20},
    {PA_DISSOLVE, 7},
    {PA_BLINDS, 7},
    {PA_RANDOM, 3},
#endif
#if ENA_WIPE
    {PA_WIPE, 5},
    {PA_WIPE_CURSOR, 4},
#endif
#if ENA_SCAN
    {PA_SCAN_HORIZ, 4},
    {PA_SCAN_HORIZX, 4},
    {PA_SCAN_VERT, 3},
    {PA_SCAN_VERTX, 3},
#endif
#if ENA_OPNCLS
    {PA_OPENING, 3},
    {PA_OPENING_CURSOR, 4},
    {PA_CLOSING, 3},
    {PA_CLOSING_CURSOR, 4},
#endif
#if ENA_SCR_DIA
    {PA_SCROLL_UP_LEFT, 7},
    {PA_SCROLL_UP_RIGHT, 7},
    {PA_SCROLL_DOWN_LEFT, 7},
    {PA_SCROLL_DOWN_RIGHT, 7},
#endif
#if ENA_GROW
    {PA_GROW_UP, 7},
    {PA_GROW_DOWN, 7},
#endif
};

/**
 * спрайт пакмана
 */
const uint8_t F_PMAN1 = 6;
const uint8_t W_PMAN1 = 8;
static const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] = // пакман
    {
        0x00,
        0x81,
        0xc3,
        0xe7,
        0xff,
        0x7e,
        0x7e,
        0x3c,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x3c,
        0x7e,
        0xff,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
};

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
static const uint8_t PROGMEM
    pacman2[F_PMAN2 * W_PMAN2] = // призрак, преследующий пакмана
    {
        0x00,
        0x81,
        0xc3,
        0xe7,
        0xff,
        0x7e,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x7b,
        0xf3,
        0x7f,
        0xfb,
        0x73,
        0xfe,
        0x3c,
        0x7e,
        0xff,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x73,
        0xfb,
        0x7f,
        0xf3,
        0x7b,
        0xfe,
        0x24,
        0x66,
        0xe7,
        0xff,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x73,
        0xfb,
        0x7f,
        0xf3,
        0x7b,
        0xfe,
        0x00,
        0x42,
        0xe7,
        0xe7,
        0xff,
        0xff,
        0x7e,
        0x3c,
        0x00,
        0x00,
        0x00,
        0xfe,
        0x73,
        0xfb,
        0x7f,
        0xf3,
        0x7b,
        0xfe,
};

/**
 * переменные для анимации и индикации
 */
int rnd;
String printTime;
char buf[256];
int cntDispScrollAnimation = 0;
int cntTextScroll = 0;
int cntDotAnimation = 0;
bool fStartScrollAnimation = 0; // флаг старта бегущей строки НЕ активен
unsigned long timer;

/**
 * переменные для времени
 */
String y;   // год
String mon; // месяц
String wd;  // день недели
String md;  // день месяца
String h;   // часы
String m;   // минуты
String s;   // секунды
uint8_t second;
uint8_t hour;
uint8_t minute;

/**
 * переменные для погоды
 */
//HTTPClient https; // создаем экземпляр класса HttpClient
//String regionID = "11002";



String tempStr;
String weatherDescription;
uint8_t getW = 0;

/**
 * объявление пользовательских функций
 */
void getTime();


void displayTime(String);
void displayTimeStart(String);
void displayTimeEnd(String);
void displayScroll(String);
void displayScrollInCycle(String, int);
void displayDotsTimeAnimation();
bool startCP(IPAddress &);

/**
 * индикация времени
 */
void displayTime(String data)
{
  if (P.displayAnimate())
  { // если анимация закончена
    P.setFont(_6bite_rus);
    data.toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, 10, 10, PA_PRINT, PA_NO_EFFECT);
  }
}

/**
 * старт анимации времени
 */
void displayTimeStart(String data)
{
  if (P.displayAnimate())
  { // если анимация закончена
    P.setFont(_6bite_rus);
    data.toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 50, catalog[rnd].effect,
                  PA_NO_EFFECT);
    if (!P.displayAnimate())
      cntDispScrollAnimation = 0;
    fStartScrollAnimation = 0;
  }
}

/**
 * завершение анимации времени
 */
void displayTimeEnd(String data)
{
  if (P.displayAnimate())
  { // если анимация закончена
    P.setFont(_6bite_rus);
    data.toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 50, PA_PRINT,
                  catalog[rnd].effect); // выводим завершение анимации
    if (!P.displayAnimate())
      cntDispScrollAnimation = 1;
  }
}

/**
 * вывод бегущей строки
 */
void displayScroll(String data)
{
  if (P.displayAnimate())
  { // если анимация закончена
    P.setFont(_5bite_rus);
    data.toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 30);
  }
}

/**
 * вывод бегущей строки в цикле
 */
void displayScrollInCycle(String data, int n)
{
  if (P.displayAnimate())
  { // если анимация закончена
    P.setFont(_5bite_rus);
    data.toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 30);
    if (!P.displayAnimate())
      cntDispScrollAnimation = 2;
    cntTextScroll = n;
  }
}

/**
 * анимация точки
 */
void displayDotsTimeAnimation()
{
  if (millis() - timer > 250)
  {
    timer = millis();
    switch (cntDotAnimation)
    { //
    case 0:
      printTime = h + "\x0c2" + m;
      break;
    case 1:
      printTime = h + "\x03a" + m;
      break;
    case 2:
      printTime = h + "\x03a" + m;
      break;
    case 3:
      printTime = h + "\x0c3" + m;
      break;
    case 4:
      printTime = h + "\x03a" + m;
      break;
    case 5:
      printTime = h + "\x03a" + m;
      break;
    case 6:
      printTime = h + "\x0c4" + m;
      break;
    case 7:
      printTime = h + "\x03a" + m;
      break;
    case 8:
      printTime = h + "\x03a" + m;
      break;
    case 9:
      printTime = h + "\x0c5" + m;
      break;
    case 10:
      printTime = h + "\x03a" + m;
      break;
    case 11:
      printTime = h + "\x03a" + m;
      break;
    }
    cntDotAnimation++;
    if (cntDotAnimation > 11)
      cntDotAnimation = 0;
  }
}

/**
 * запрос времени
 */
void getTime()
{
  time_t now = time(nullptr);
  const struct tm *timeinfo = localtime(&now);

  minute = timeinfo->tm_min;
  hour = timeinfo->tm_hour;
  second = timeinfo->tm_sec;

  h = String(hour / 10) + String(hour % 10);
  m = String(minute / 10) + String(minute % 10);
  s = String(second / 10 + String(second % 10));
  md = String(timeinfo->tm_mday);
  y = String(timeinfo->tm_year + 1900);

  switch (timeinfo->tm_mon)
  { // месяц [ 0-январь - 11-декабрь ]
  case 0:
    mon = "января";
    break;
  case 1:
    mon = "февраля";
    break;
  case 2:
    mon = "марта";
    break;
  case 3:
    mon = "апреля";
    break;
  case 4:
    mon = "мая";
    break;
  case 5:
    mon = "июня";
    break;
  case 6:
    mon = "июля";
    break;
  case 7:
    mon = "августа";
    break;
  case 8:
    mon = "сентября";
    break;
  case 9:
    mon = "октября";
    break;
  case 10:
    mon = "ноября";
    break;
  case 11:
    mon = "декабря";
    break;
  }

  switch (timeinfo->tm_wday)
  { // день недели [ 0-воскресенье - 6-суббота ]
  case 0:
    wd = "воскресенье";
    break;
  case 1:
    wd = "понедельник";
    break;
  case 2:
    wd = "вторник";
    break;
  case 3:
    wd = "среда";
    break;
  case 4:
    wd = "четверг";
    break;
  case 5:
    wd = "пятница";
    break;
  case 6:
    wd = "суббота";
    break;
  }
}

void setup()
{
  // Открываем последовательный порт, устанавливаем скорость 115200 бит/c
  Serial.begin(115200);
  Serial.println("Booting...");

  // Hастройка анимации MD_Parola
  P.begin();
  P.setInvert(false);

#ifdef DEBUG
  P.setIntensity(1);
#endif
#if ENA_SPRITE
  P.setSpriteData(pacman1, W_PMAN1, F_PMAN1, pacman2, W_PMAN2, F_PMAN2);
#endif
  for (uint8_t i = 0; i < ARRAY_SIZE(catalog); i++)
  {
    catalog[i].speed *= P.getSpeed();
  }
  // Подключение к WiFi
  connectWiFi();

  // Настройка времени
  configTime(MYTZ, "192.168.1.1", "ru.pool.ntp.org");
  Serial.println("Witing for NTP time sync:");
  int i = 0;
  time_t now = time(nullptr);
  while (now < 1000000000)
  {
    now = time(nullptr);
    i++;
    if (i > 60)
    {
      Serial.println("");
      Serial.println("Time sync failed!");
    }
    Serial.println(".");
    delay(500);
  }

  // Эапрос данных о погоде
  getWeatherData();
}

void loop()
{
  getTime(); // запрос времени
  if (minute > 0 && !(minute % 19) && second == 20 &&
      getW == 0)
  { // 3 раза в час запрос погоды
    getWeatherData();
    getW = 1;
  }
  else
    getW = 0;

  if (second > 0 && !(second % 35) && fStartScrollAnimation == 0)
    fStartScrollAnimation =
        1; // если таймер бегущей строки сработал
           // и при этом флаг старта бегущей строки не установлен,
           //  устанавливаем его
#ifndef DEBUG
#ifdef Night_Bbrightness // включение ночного режима яркости
  if (hour < 6)
    P.setIntensity(2);
  else if (hour >= 6 && hour < 20)
    P.setIntensity(15);
  else if (hour >= 20)
    P.setIntensity(7);
#endif
#endif
  /**
   * смена экранов анимации:
   * флаг старта бегущей строки   1                             0
   * экран 0               завершение времени            индикация времени
   * экран 1                 бегущая строка              индикация времени
   * экран 2                  старт времени              индикация времени
   *
   * текст бегущей строки чередуется по счетчику cntTextScroll
   */
  if (cntDispScrollAnimation == 0 &&
      fStartScrollAnimation)
  { // если флаг старта бегущей строки установлен
    // и счетчик экранов анимации равен 0
    rnd = random(0, ARRAY_SIZE(catalog));
    displayDotsTimeAnimation();
    displayTimeEnd(printTime); // выводим анимацию завершения экрана времени
  }
  if (cntDispScrollAnimation == 1 &&
      fStartScrollAnimation)
  { // если флаг старта бегущей строки установлен
    // и счетчик экранов анимации равен 1
    if (cntTextScroll == 0)
    { // при счетчике строк бегущей строки равном 0
      displayScrollInCycle((wd + " " + md + " " + mon),
                           1); // выводим первую бегущую строку
    }
    if (cntTextScroll == 1)
    { // при счетчике строк бегущей строки равном 1
      displayScrollInCycle((global::temp),0); // выводим вторую бегущую строку
              // и т.д.
    }
  }
  if (cntDispScrollAnimation == 2 &&
      fStartScrollAnimation)
  { // если флаг старта бегущей строки установлен
    // и счетчик экранов анимации равен 2
    displayDotsTimeAnimation();
    displayTimeStart(printTime); // выводим анимацию старта экрана времени
  }
  if (!fStartScrollAnimation)
  { // если флаг старта бегущей строки не
    // установлен
    displayDotsTimeAnimation();
    displayTime(printTime); // выводим время
  }
}