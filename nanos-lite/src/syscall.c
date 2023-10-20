#include <common.h>
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
    case SYS_write:
      assert(a[1] == 1 || a[1] == 2);
      printf("len : %d\n", a[3]);
      char* buf = (char *)a[2];
      for (int i = 0; i < a[3]; i++) {
        putch(buf[i]);
      }
      SET_RETURN(a[3]);
      break;
    case SYS_brk:
      SET_RETURN(0);
      break;
      
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
