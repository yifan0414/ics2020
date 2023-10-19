#include <common.h>
#include <syscall.h>

#define SET_RETURN(val) c->GPRx = val;
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit:
      halt(0);
      break;
    case SYS_yield:
      yield();
      SET_RETURN(0);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
