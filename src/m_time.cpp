#include "m_time.h"

void Realtime::getTime()
{
    time_t now = time(nullptr);
    timeinfo = localtime(&now);
    hour = timeinfo->tm_hour;
    minute = timeinfo->tm_min;
    second = timeinfo->tm_sec;
    y = String(timeinfo->tm_year + 1900);
    md = String(timeinfo->tm_mday);
    h = String(timeinfo->tm_hour / 10) + String(timeinfo->tm_hour % 10);
    m = String(timeinfo->tm_min / 10) + String(timeinfo->tm_min % 10);
    s = String(timeinfo->tm_sec / 10) + String(timeinfo->tm_sec % 10);
    mon = mounth(timeinfo->tm_mon);
    wd = dayOfTheWeek(timeinfo->tm_wday);
}

String Realtime::mounth(int var)
{
    u8_t i{11};
    while (i > var)
    {
        i--;
    }
    return _mounth[i];
}

String Realtime::dayOfTheWeek(int var)
{
    u8_t i{6};
    while (i > var)
    {
        i--;
    }
    return _dayOfTheWeek[i];
}