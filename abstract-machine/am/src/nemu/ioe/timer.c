#include <am.h>
#include <nemu.h>

uint64_t boot_time;

void __am_timer_init() {
  boot_time = (uint64_t)inl(RTC_ADDR + 4) * 1000000LL + inl(RTC_ADDR);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  // AM_TIMER_UPTIME, AM 系统启动时间, 可读出系统启动后的微秒数
  // 怎么读出呢？
  // 在nemu中的timer.c 中模拟了时钟
  // gettimeofday(&now, NULL);
  // rtc_port_base[0] = now.tv_usec;
  // rtc_port_base[1] = now.tv_sec;
  uptime->us =
      (uint64_t)inl(RTC_ADDR + 4) * 1000000LL + inl(RTC_ADDR) - boot_time;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour = 0;
  rtc->day = 0;
  rtc->month = 0;
  rtc->year = 1900;
}
