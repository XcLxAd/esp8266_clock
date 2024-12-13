#pragma once

#include <Arduino.h>

#include <TZ.h>
#include <sntp.h>
#include <time.h>
// установка временной зоны
#ifndef MYTZ
#define MYTZ TZ_Europe_Moscow
#endif

class Realtime
{
private:
    struct tm *timeinfo;

public:
    int int_sec;
    int int_hour;
    int int_min;
    String str_year; // год
    String str_mon;  // месяц года
    String str_wday; // день недели
    String str_mday; // день месяца
    String str_hour; // часы
    String str_min;  // минуты
    String str_sec;  // секунды

    void getTime();
};
