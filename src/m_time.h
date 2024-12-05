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
    const char *_mounth[12] =
        {
            "января",
            "февраля",
            "марта",
            "апреля",
            "мая",
            "июня",
            "июля",
            "августа",
            "сентября",
            "октября",
            "ноября",
            "декабря"};
            
    const char *_dayOfTheWeek[7] =
        {
            "Воскресенье",
            "Понедельник",
            "Вторник",
            "Среда",
            "Четверг",
            "Пятница",
            "Суббота"};

    String mounth(int);
    String dayOfTheWeek(int);
    const struct tm *timeinfo;

public:
    int second;
    int hour;
    int minute;
    String y;   // год
    String mon; // месяц
    String wd;  // день недели
    String md;  // день месяца
    String h;   // часы
    String m;   // минуты
    String s;   // секунды

    void getTime();
};
