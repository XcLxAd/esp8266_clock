#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPClient.h>
#include <iostream>
#include <string.h>
#include <sstream>
#include <SPI.h>
#include <Wire.h>
#include <WiFiClientSecureBearSSL.h>
// подключение библиотек работы со временем
#include <sntp.h>
#include <time.h>
#include <TZ.h>
// подключение библиотек работы с матрицей и эффектов
#include "MD_MAX72xx.h"
#include "MD_Parola.h"
// подключение шрифтов
#include "5bite_rus.h"
#include "6bite_rus.h"


// установка временной зоны
#define MYTZ TZ_Europe_Moscow
// установка имени и пароля точки доступа 
#ifndef STASSID
  #define STASSID "Home_Network"
  #define STAPSK  "5VjK-8ePo-aysa-mkyr-567-bhd"
#endif

// установки индикации
#define MAX_DEVICES 4 // количество матриц
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW // используются матрицы типа FC-16
/* подключение пинов Wemos D1 mini
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
#define PHOTORESISTOR_PIN A0 // A0/ADC0



/**
 *  настройка анимации
*/
MD_Parola P = MD_Parola(HARDWARE_TYPE, MAX7219_CS_PIN, MAX_DEVICES); // выбор интерфейса SPI

struct sCatalog {
  textEffect_t effect;   // text effect to display
  uint16_t speed;    // speed multiplier of library default
};

sCatalog catalog[] = { 
    { PA_SCROLL_UP, 5 },
    { PA_SCROLL_DOWN, 5 },
    { PA_SCROLL_LEFT, 5 },
    { PA_SCROLL_RIGHT, 5, },
  #if ENA_SPRITE
    { PA_SPRITE, 5 },
  #endif
  #if ENA_MISC
    { PA_SLICE, 1 },
    { PA_MESH, 20 },
    { PA_FADE, 20 },
    { PA_DISSOLVE, 7 },
    { PA_BLINDS, 7 },
    { PA_RANDOM, 3 },
  #endif    
  #if ENA_WIPE
    { PA_WIPE, 5 },
    { PA_WIPE_CURSOR, 4 },
  #endif
  #if ENA_SCAN
    { PA_SCAN_HORIZ, 4 },
    { PA_SCAN_HORIZX, 4 },
    { PA_SCAN_VERT, 3 },
    { PA_SCAN_VERTX, 3 },
  #endif  
#if ENA_OPNCLS
    { PA_OPENING, 3 },
    { PA_OPENING_CURSOR, 4 },
    { PA_CLOSING, 3 },
    { PA_CLOSING_CURSOR, 4 },
#endif
#if ENA_SCR_DIA
    { PA_SCROLL_UP_LEFT, 7 },
    { PA_SCROLL_UP_RIGHT, 7 },
    { PA_SCROLL_DOWN_LEFT, 7 },
    { PA_SCROLL_DOWN_RIGHT, 7 },
#endif
#if ENA_GROW
    { PA_GROW_UP, 7 },
    { PA_GROW_DOWN, 7 },
#endif
};

// определение спрайта
const uint8_t F_PMAN1 = 6;
const uint8_t W_PMAN1 = 8;
static const uint8_t PROGMEM pacman1[F_PMAN1 * W_PMAN1] =  // gobbling pacman animation
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c,
};

const uint8_t F_PMAN2 = 6;
const uint8_t W_PMAN2 = 18;
static const uint8_t PROGMEM pacman2[F_PMAN2 * W_PMAN2] =  // ghost pursued by a pacman
{
  0x00, 0x81, 0xc3, 0xe7, 0xff, 0x7e, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe,
  0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
  0x24, 0x66, 0xe7, 0xff, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
  0x00, 0x42, 0xe7, 0xe7, 0xff, 0xff, 0x7e, 0x3c, 0x00, 0x00, 0x00, 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe,
};




// присвоение строковым константам значений имени и пароля точки доступа
const String ssid = STASSID;
const String password = STAPSK;
char ipAddressChr [16];
static std::stringstream apiURL; // создаем строчный потоковый класс


/**
 * 
 */
int rnd;
bool fStartScrollAnimation = 0; // флаг старта бегущей строки НЕ активен
bool fStartAnimationTime = 1; // флаг анимации старта времени Активен
bool fEndAnimationTime = 1; // флаг анимации завершения времени Активен

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
uint8_t hour;
uint8_t minute;

/**
 * 
 */ 
HTTPClient http;  // создаем экземпляр класса HttpClient
std::string server = "https://api.climacell.co/v3/weather/realtime";
const float_t lat     = 45.855;
const float_t lon     = 40.125;
String apiKey  = "NLOXCpsbWaf689MVyFuKZSrBpNJBesm1";


String temp;
uint8_t getW = 0;

/**
 * преобразование ответа о погоде в данные
 */
const int jsonSize = JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(1);
String parseJson(String jsonDoc) {
  Serial.println("Parsing JSON...");
  DynamicJsonDocument doc(jsonSize);
  deserializeJson(doc, jsonDoc);
  String value = doc["temp"]["value"];
  return value;
}

/**
 * запрос погоды
 */
void getWeatherData() {
  Serial.println("Connecting to the HTTP server....");
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
  client->setInsecure();
  Serial.println("Building URL...");
  apiURL << server << "?lat=" << lat << "&lon=" << lon << "&fields=" << "temp"; // заполняем строчный потоковый класс данными 
  Serial.printf("API URL = %s\r\n", apiURL.str().c_str()); 
  if (http.begin(*client, apiURL.str().c_str())) {
    Serial.println("Connected");
    http.addHeader("Accept", "application/json"); // метод addHeader передант в качестве первого параметра имя заголовка и как второй параметр значение заголовка.
    http.addHeader("apikey", apiKey);
    int code = http.GET();

    Serial.printf("HTTP Code [%d]", code);
    if (code > 0) {
      if (code == HTTP_CODE_OK || code == HTTP_CODE_MOVED_PERMANENTLY) {
        Serial.println(" GET OK");
        String payload = http.getString(); // ответ содержит данные JSON в результате вызова API
        Serial.print("JSON: ");
        Serial.println(payload);
        temp = parseJson(payload);
        getW = 1;
        Serial.print("Temperature = ");
        Serial.println(temp);
        apiURL.str(""); // очищаем буфер        
      }
    }
    else {
      Serial.printf("[HTTP] GET... failed, error: %s", http.errorToString(code).c_str());
    }
  }
}






 // индикация времени
void displayTime() {
  if (P.displayAnimate()) { // если анимация закончена
    P.setFont(_6bite_rus);
    P.displayText(Text.c_str(), PA_CENTER, 200, 0, PA_PRINT, PA_NO_EFFECT);
    if (!(second % 55) && !(hour == 7)) fStartScrollAnimation = 1;  // каждые nn секунд устанавливаем флаг анимации бегущей строки      
  }
}

 // старт анимации времени 
void displayTimeStart() {
  if (P.displayAnimate()) { // если анимация закончена
    rnd = random(0, ARRAY_SIZE(catalog));
    P.setFont(_6bite_rus);
    Text.toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 0, catalog[rnd].effect, PA_NO_EFFECT);
    fStartAnimationTime = 0; // флаг анимации старта времени НЕ активен
    fEndAnimationTime = 1; // флаг анимации завершения времени Активен
    if (!(second % 55) && !(hour == 7)) fStartScrollAnimation = 1; // каждые nn секунд устанавливаем флаг анимации бегущей строки  
  }
}

 // завершение анимации времени
void displayTimeEnd() {
  if (P.displayAnimate()) { // если анимация закончена
    rnd = random(0, ARRAY_SIZE(catalog));
    P.setFont(_6bite_rus);
    Text.toCharArray(buf, 256);
    P.displayText(buf, PA_CENTER, catalog[rnd].speed, 0, PA_PRINT, catalog[rnd].effect); // выводим завершение анимации
    fEndAnimationTime = 0; // флаг анимации завершения времени НЕ активен
   }
}

 // вывод бегущей строки
void displayScroll() {
  if (P.displayAnimate()) { // если анимация закончена
    P.setFont(_5bite_rus);
    Text.toCharArray(buf, 256);
    P.displayScroll(buf, PA_LEFT, PA_SCROLL_LEFT, 30); 
    fStartScrollAnimation = 0; // флаг старта бегущей строки НЕ активен
    fStartAnimationTime = 1; // флаг анимации старта времени Активен
  }
}



/**
 * запрос времени
 */
void getTime() {
  time_t now = time(nullptr);
  const struct tm * timeinfo = localtime(&now);
  
  minute = timeinfo->tm_min;
  hour = timeinfo->tm_hour;
  second = timeinfo->tm_sec;
  
  h = String (hour/10) + String (hour%10);
  m = String (minute/10) + String (minute%10);
  s = String (second/10 + String (second%10));
  md = String (timeinfo->tm_mday);
  y = String (timeinfo->tm_year + 1900);
  
  switch(timeinfo->tm_mon) {  // месяц [ 0-январь - 11-декабрь ]
    case 0: 
      mon = "января"; break;
    case 1:
      mon = "февраля"; break;
    case 2:
      mon = "марта"; break;
    case 3:
      mon = "апреля"; break;
    case 4:
      mon = "мая"; break;
    case 5:
      mon = "июня"; break;
    case 6:
      mon = "июля"; break;
    case 7:
      mon = "августа"; break;
    case 8:
      mon = "сентября"; break;
    case 9:
      mon = "октября"; break;
    case 10:
      mon = "ноября"; break;
    case 11:
      mon = "декабря"; break;
  }

  switch(timeinfo->tm_wday) { // день недели [ 0-воскресенье - 6-суббота ]
    case 0:
      wd = "воскресенье"; break;  
    case 1:
      wd = "понедельник"; break;
    case 2:
      wd = "вторник"; break;
    case 3:
      wd = "среда"; break;
    case 4:
      wd = "четверг"; break;
    case 5:
      wd = "пятница"; break;
    case 6:
      wd = "суббота"; break;
  }
}

/**
 *поместите свой установочный код сюда, чтобы запустить один раз: 
 */
void setup() {
  // настройка порта 
  Serial.begin(9600);
  Serial.println("Booting");
  
  // настройка WIFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  
  // настройка Arduino OTA
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {
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
  
  //настройка анимации MD_Parola
  P.begin();
  P.setInvert(false);
  #if ENA_SPRITE
    P.setSpriteData(pacman1, W_PMAN1, F_PMAN1, pacman2, W_PMAN2, F_PMAN2);
  #endif
  for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++) {
    catalog[i].speed *= P.getSpeed();
  }
  
  // вывод на экран IP адреса
  
  Text = WiFi.localIP().toString();
  displayScroll();

  // настройка времени
  configTime(MYTZ, "time.nist.gov", "time.windows.com", "ru.pool.ntp.org");
  while(!sntp_enabled()) {
    Serial.print("-");
    delay(100);
  }
  
  // однократный запрос данных о погоде
  getWeatherData();
  delay(100);
}

/**
 * основная программа
 */
void loop() {
  // Arduino OTA 
  ArduinoOTA.handle();
  
  // в полночь запрос времени
  if (!hour && !minute && !second) sntp_init(); 
  
  // 3 раза в час запрос погоды
  if (minute > 0 && !(minute % 19) && second == 20 && getW == 0) {
    getWeatherData();
    getW = 1;
  } 
  else getW = 0;
  
  // запрс времени
  getTime();
 
  // индикация
  if (second & 1) Text = h + ":" + m;
  else Text = h + " " + m;
  
  if (fStartScrollAnimation) { // если флаг старта анимации бегущей строки активен
    if (fEndAnimationTime) displayTimeEnd(); // если флаг завершения анимации времени активен запускаем анимацию
    else { // иначе выводим бегущую строку
      
      Text = wd + " " + md + " " + mon + " t = " + temp + "\x0f7" + "C";      
      displayScroll();
    }
  }
// 
  else { // если флаг старта анимации бегущей строки НЕ активен
    if (fStartAnimationTime) displayTimeStart();   // если флаг старта анимации времени активен запускаем анимацию
    else displayTime(); // иначе выводим время
  }


 



}