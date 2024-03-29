#include <monitor/difftest.h>

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

void raise_intr(DecodeExecState *s, word_t NO, vaddr_t ret_addr);
void restore_intr(DecodeExecState *s);

static inline def_EHelper(lidt) {
  // BUG：注意这里不能直接使用 ddest, 无意义
  rtl_lms(s, &cpu.IDTR.limit, s->isa.mbase, 0, 2);
  rtl_lm(s, &cpu.IDTR.base, s->isa.mbase, 2, 4);
  print_asm_template1(lidt);
}

static inline def_EHelper(mov_r2cr) {
  TODO();
  print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {
  TODO();
  print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(int) {
  raise_intr(s, *ddest, s->seq_pc);
  
  print_asm("int %s", id_dest->str);

#ifndef __DIFF_REF_NEMU__
  // BUG: if use difftest_skip_dut(1, 2), pc for qemu will strange
  // difftest_skip_dut(1, 2);
  difftest_skip_ref();
#endif
}

static inline def_EHelper(iret) {
  restore_intr(s);
  print_asm("iret");

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(in) {
  switch (id_dest->width) {
    case 1:
      rtl_li(s, s0, pio_read_b(*dsrc1));
      break;
    case 2:
      rtl_li(s, s0, pio_read_w(*dsrc1));
      break;
    case 4:
      rtl_li(s, s0, pio_read_l(*dsrc1));
  }
  operand_write(s, id_dest, s0);

  print_asm_template2(in);
}

static inline def_EHelper(out) {
  cpu.gpr[1]._16 = cpu.gpr[1]._8[0];
  switch (id_dest->width) {
    case 1:
      pio_write_b(*ddest, *dsrc1);
      break;
    case 2:
      pio_write_w(*ddest, *dsrc1);
      break;
    case 4:
      pio_write_l(*ddest, *dsrc1);
  }

  print_asm_template2(out);
}
