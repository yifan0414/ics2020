#include <common.h>
#include <fs.h>
#include <syscall.h>

#define SET_RETURN(val) c->GPRx = val;
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
  case SYS_exit:
    halt(0);
    break;

  case SYS_yield:
    yield();
    SET_RETURN(0);
    break;

  case SYS_open:
    SET_RETURN(fs_open((const char *)a[1], a[2], a[3]));
    break;

  case SYS_read:
    SET_RETURN(fs_read(a[1], (void *)a[2], a[3]));
    break;

  case SYS_write:
    SET_RETURN(fs_write(a[1], (void *)a[2], a[3]));
    break;

  case SYS_close:
    SET_RETURN(fs_close(a[1]));
    break;

  case SYS_lseek:
    SET_RETURN(fs_lseek(a[1], a[2], a[3]));
    break;

  case SYS_brk:
    SET_RETURN(0);
    break;

  case SYS_gettimeofday:;
    struct timeval {
      long int tv_sec;  /* seconds */
      long int tv_usec; /* and microseconds */
    };
    uint64_t time = io_read(AM_TIMER_UPTIME).us;
    ((struct timeval *)a[1])->tv_usec = time % 1000000;
    ((struct timeval *)a[1])->tv_sec = time / 1000000;
    SET_RETURN(0);
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
