#ifndef __CC_H__
#define __CC_H__

#include "../local-include/rtl.h"

enum {
  CC_O, CC_NO, CC_B,  CC_NB,
  CC_E, CC_NE, CC_BE, CC_NBE,
  CC_S, CC_NS, CC_P,  CC_NP,
  CC_L, CC_NL, CC_LE, CC_NLE
};


/* Condition Code */

static inline const char* get_cc_name(int subcode) {
  static const char *cc_name[] = {
    "o", "no", "b", "nb",
    "e", "ne", "be", "nbe",
    "s", "ns", "p", "np",
    "l", "nl", "le", "nle"
  };
  return cc_name[subcode];
}

// static inline void rtl_setcc(DecodeExecState *s, rtlreg_t* dest, uint32_t subcode) {
//   uint32_t invert = subcode & 0x1;
//   // 这里是因为 指令的分布是 SETO SETNO SETB SETNB, 因此我们只需要判断最后以为是否为1即可来区分是否需要取反

//   // TODO: Query EFLAGS to determine whether the condition code is satisfied.
//   // dest <- ( cc is satisfied ? 1 : 0)
//   // 1110
//   switch (subcode & 0xe) {
//     case CC_O: if (cpu.eflags.OF == 1) *dest = 1; else *dest = 0; break;
//     case CC_B: if (cpu.eflags.CF == 1 && cpu.eflags.ZF == 0) *dest = 1; else *dest = 0; break; //cpu.eflags.ZF == 0 bug!
//     case CC_E: if (cpu.eflags.ZF == 1) *dest = 1; else *dest = 0; break;
//     case CC_BE:if (cpu.eflags.CF == 1 || cpu.eflags.ZF == 1) *dest = 1; else *dest = 0; break;
//     case CC_S: if (cpu.eflags.SF == 1) *dest = 1; else *dest = 0; break;
//     case CC_L: if (cpu.eflags.SF != cpu.eflags.OF && cpu.eflags.ZF == 0) *dest = 1; else *dest = 0; break;
//     case CC_LE:if (cpu.eflags.SF != cpu.eflags.OF || cpu.eflags.ZF == 1) *dest = 1; else *dest = 0; break;
//     case CC_P: panic("PF is not supported");
//     default: panic("should not reach here");
//   }

//   if (invert) {
//     rtl_xori(s, dest, dest, 0x1);
//   }
//   assert(*dest == 0 || *dest == 1);
// }

static inline void rtl_setcc(DecodeExecState *s, rtlreg_t* dest, uint32_t subcode) {
  uint32_t invert = subcode & 0x1;

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {
    case CC_O:
      rtl_get_OF(s, dest);
      break;
    case CC_B:
      rtl_get_CF(s, dest);
      break;
    case CC_E:
      rtl_get_ZF(s, dest);
      break;
    case CC_BE:
      rtl_get_CF(s, s1);
      rtl_get_ZF(s, s2);
      rtl_or(s, dest, s1, s2);
      break;
    case CC_S:
      rtl_get_SF(s, dest);
      break;
    case CC_L:
      rtl_get_SF(s, s1);
      rtl_get_OF(s, s2);
      rtl_xor(s, dest, s1, s2);
      break;
    case CC_LE:
      rtl_get_ZF(s, t0);
      rtl_get_SF(s, s1);
      rtl_get_OF(s, s2);
      rtl_xor(s, s0, s1, s2);
      rtl_or(s, dest, t0, s0);
      break;
    default: panic("should not reach here");
    case CC_P: panic("PF is not supported");
  }

  if (invert) {
    rtl_xori(s, dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
}


#endif
