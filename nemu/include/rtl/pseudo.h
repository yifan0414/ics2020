#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

// #ifndef __RTL_RTL_H__
// #error "Should be only included by <rtl/rtl.h>"
// #endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t *dest, const rtlreg_t imm) {
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t *dest, const rtlreg_t *src1) {
  if (dest != src1) rtl_add(s, dest, src1, rz);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t *src1) {
  // dest <- ~src1
  *dest = ~(*src1);
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t *src1) {
  // dest <- -src1
  *dest = -(*src1);
}

static inline def_rtl(sext, rtlreg_t *dest, const rtlreg_t *src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  rtlreg_t mask = (1ull << (width * 8)) - 1;
  rtlreg_t sign = 1ull << (width * 8 - 1);
  *dest = (*src1 & mask) - (*src1 & sign) * 2;
}

static inline def_rtl(zext, rtlreg_t *dest, const rtlreg_t *src1, int width) {
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  rtl_li(s, dest, (*src1 << (32 - 8 * width)) >> (32 - 8 * width));
}

static inline def_rtl(msb, rtlreg_t *dest, const rtlreg_t *src1, int width) {
  // dest <- src1[width * 8 - 1]
  rtl_li(s, dest, (*src1 >> (width * 8 - 1)) & 1);
}

#endif
