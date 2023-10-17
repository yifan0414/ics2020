#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  vaddr_t idtr_gate = cpu.IDTR.base + 8 * NO;
  vaddr_t idtr_addr = (vaddr_read(idtr_gate + 4, 4) & 0xffff0000) +
                      (vaddr_read(idtr_gate, 4) & 0x0000ffff);
  // assert(target >= 0x00100663 && target <= 0x00100675);
  rtl_push(s, &cpu.eflags.val);
  cpu.eflags.IF = 0;

  rtl_li(s, s0, cpu.cs);
  rtl_push(s, s0);

  rtl_li(s, s0, ret_addr);
  rtl_push(s, s0);

  rtl_j(s, idtr_addr);
}

void restore_intr(DecodeExecState *s) {
  rtl_pop(s, s0);
  rtl_jr(s, s0);

  rtl_pop(s, s0);
  cpu.cs = *s0;

  rtl_pop(s, &cpu.eflags.val);
}

void query_intr(DecodeExecState *s) {
  TODO();
}
