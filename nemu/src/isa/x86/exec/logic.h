#include "cc.h"

static inline def_EHelper(test) {
  rtl_and(s, s0, ddest, dsrc1);
  rtl_set_CF(s, rz);
  rtl_set_OF(s, rz);
  rtl_update_ZFSF(s, s0, id_dest->width);
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  rtl_and(s, ddest, ddest, dsrc1);
  rtl_set_CF(s, rz);
  rtl_set_OF(s, rz);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(and);
}

static inline def_EHelper(xor) {
  rtl_xor(s, ddest, ddest, dsrc1);
  rtl_li(s, s0, 0);
  rtl_set_CF(s, s0);
  rtl_set_OF(s, s0);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(xor);
}

static inline def_EHelper(or) {
  rtl_or(s, ddest, ddest, dsrc1);
  rtl_set_CF(s, rz);
  rtl_set_OF(s, rz);
  rtl_update_ZFSF(s, ddest, id_dest->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(or);
}

static inline def_EHelper(not) {
  rtl_not(s, ddest, ddest);
  operand_write(s, id_dest, ddest);
  print_asm_template1(not);
}

// BUG: 16位数算术右移的高位
static inline def_EHelper(sar) {
  rtl_sar(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  rtl_shl(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  rtl_shr(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shr);
}


static inline def_EHelper(setcc) {
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, ddest, cc);
  operand_write(s, id_dest, ddest);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

static inline def_EHelper(rol) {
  // printf("rol ddest 0x%x(width %d); dsrc1 0x%x ", *ddest, id_dest->width, *dsrc1, id_src2->width);
  // if (*dsrc1 != 0 && *dsrc1 < id_dest->width * 8 && *dsrc1 <= id_dest->width * 8)
  *ddest = (*ddest << *dsrc1) | (*ddest >> (id_dest->width * 8 - *dsrc1));
  // printf("result 0x%x\n", *ddest);
  operand_write(s, id_dest, ddest);
  rtl_update_ZFSF(s, ddest, id_dest->width);

  print_asm_template2(rol);
}