/*
 * time.h
 *
 *  Created on: Sep 30, 2019
 *      Author: blward
 */

#ifndef INC_TIME_H_
#define INC_TIME_H_

#include <main.h>
#include <stdint.h>

struct Date
{
    uint32_t month;
    uint32_t day;
};

struct Time
{
    uint32_t hours;
    uint32_t minutes;
    uint32_t seconds;
};

const char* monthNames[] = { "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL",
                             "AUG", "SEP", "OCT", "NOV", "DEC" };
const uint32_t monthDayCounts[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30,
                                    31 };

const uint32_t HOURS_PER_DAY = 24, MINUTES_PER_HOUR = 60, SECONDS_PER_MINUTE =
        60;
const uint32_t SECONDS_PER_DAY = HOURS_PER_DAY * MINUTES_PER_HOUR
        * SECONDS_PER_MINUTE;

uint32_t monthToSeconds(uint8_t index)
{
    if (index > 11)
        return 0;
    return monthDayCounts[index] * SECONDS_PER_DAY;
}

uint32_t dayToSeconds(uint8_t day)
{
    return day * SECONDS_PER_DAY;
}

uint32_t dateToSeconds(Date date)
{
    uint32_t total = 0;
    for (int i = 0; i < date.month; i++)
        total += monthToSeconds(i);
    return total + dayToSeconds(date.day);
}

uint32_t timeToSeconds(Time time)
{
    uint32_t total = time.hours * MINUTES_PER_HOUR * SECONDS_PER_MINUTE
            + time.minutes * SECONDS_PER_MINUTE + time.seconds;
}

Time currentTime()
{
    Time time;
    time.seconds = globalClock % SECONDS_PER_MINUTE;
    time.minutes = globalClock / SECONDS_PER_MINUTE % MINUTES_PER_HOUR;
    time.hours = globalClock / SECONDS_PER_MINUTE / MINUTES_PER_HOUR
            % HOURS_PER_DAY;
    return time;
}

Date currentDate()
{
    Date date;
    int32_t day = globalClock / SECONDS_PER_DAY;
    uint8_t month = 0;
    for (int i = 0; i < 12; i++)
    {
        day -= monthDayCounts[i];
        if (day > 0)
        {
            month++;
        }
        else
        {
            day += monthDayCounts[i];
            break;
        }
    }

    date.month = month;
    date.day = (uint32_t) day;
    return date;

//    def
//    get_month2(day)
//:
//months = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
//month = 0
//for i in range(0, 12):
//day -= months[i]
//if(day > 0):
//month += 1
//else:
//day += months[i]
//break
//return (month, day)
}

uint32_t deltaNumber(uint32_t initial, uint32_t min, uint32_t max, int32_t delta, int32_t divisor) {
    uint32_t final = initial + delta/divisor;
    if(final < min)
        final = min;
    if(final > max)
        final = max;
    return final;
}

#endif /* INC_TIME_H_ */
