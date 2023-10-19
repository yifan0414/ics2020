#include <amtest.h>

void rtc_test() {
  AM_TIMER_RTC_T rtc;
  int sec = 1;
  while (1) {
    while(io_read(AM_TIMER_UPTIME).us / 1000000 < sec) ;
    rtc = io_read(AM_TIMER_RTC);
    printf("%d-%d-%d %02d:%02d:%02d GMT (", rtc.year, rtc.month, rtc.day, rtc.hour, rtc.minute, rtc.second);
    unsigned long us = io_read(AM_TIMER_UPTIME).us;
    if (sec == 1) {
      printf("%d %d second).\n",us, sec);
    } else {
      printf("%d %d seconds).\n",us, sec);
    }
    sec ++;
  }
}
