#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  // BUG: 
  // cpu.esp = cpu.esp - s->dest.width;
  // rtl_sm(s, &cpu.esp, 0, src1, s->dest.width);
  rtl_subi(s, t0, &cpu.esp, 4);
  rtl_sm(s, t0, 0, src1, 4);
  rtl_mv(s, &cpu.esp, t0);
}

static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lm(s, dest, &cpu.esp, 0, s->dest.width);
  cpu.esp = cpu.esp + s->dest.width;
}

static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  if ((is_sign(src1, width) && !is_sign(src2, width) && !is_sign(res, width)) ||
      (!is_sign(src1, width) && is_sign(src2, width) && is_sign(res, width))) {
        *dest = 0x1; 
      } else {
        *dest = 0x0;
      }
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
  *dest = (*src1 < *src2) ? 1 : 0;
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  // if (is_sign(src1, width) == is_sign(src2, width)) {
  //   *dest = is_sign(src1, width) == is_sign(res, width);
  // }
  rtlreg_t sign1 = *src1 >> (width * 8 - 1);
  rtlreg_t sign2 = *src2 >> (width * 8 - 1);
  rtlreg_t signr = *res >> (width * 8 - 1);
  *dest = (sign1 == sign2) && (sign1 != signr);
}

static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  *dest = (*res < *src1) ? 1 : 0;
}

#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    cpu.eflags.f = *src; \
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    *dest = cpu.eflags.f; \
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  switch (width) {
    case 1: cpu.eflags.ZF = is_zero((*result) & 0xff); return;
    case 2: cpu.eflags.ZF = is_zero((*result) & 0xffff); return;
    case 4: cpu.eflags.ZF = is_zero(*result); return;
    default: assert(0);
  }
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  // switch (width) {
  //   case 4: cpu.eflags.SF = (*result >> 31) & 0x1; return;
  //   case 1: cpu.eflags.SF = (*result >> 7) & 0x1; return;
  //   case 2: cpu.eflags.SF = (*result >> 15) & 0x1; return;
  //   default: assert(0);
  // }
  cpu.eflags.SF = is_sign(result, width);
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
