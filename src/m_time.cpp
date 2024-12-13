#include "m_time.h"

void Realtime::getTime()
{
    const char *_mounthOfTheYear[] =
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

    const char *_dayOfTheWeek[] =
        {
            "Воскресенье",
            "Понедельник",
            "Вторник",
            "Среда",
            "Четверг",
            "Пятница",
            "Суббота"};
            
    time_t now = time(nullptr);
    timeinfo = localtime(&now);
    int_hour = timeinfo->tm_hour;
    int_min = timeinfo->tm_min;
    int_sec = timeinfo->tm_sec;
    str_year = String(timeinfo->tm_year + 1900);
    str_mon = _mounthOfTheYear[timeinfo->tm_mon];
    str_mday = timeinfo->tm_mday;
    str_wday = _dayOfTheWeek[timeinfo->tm_wday];
    str_hour = String(timeinfo->tm_hour / 10) + String(timeinfo->tm_hour % 10);
    str_min = String(timeinfo->tm_min / 10) + String(timeinfo->tm_min % 10);
    str_sec = String(timeinfo->tm_sec / 10) + String(timeinfo->tm_sec % 10);
}
