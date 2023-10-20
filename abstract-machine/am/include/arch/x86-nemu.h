#ifndef ARCH_H__
#define ARCH_H__

// struct Context {
//   uintptr_t esi, ebx, eax, eip, edx, eflags, ecx, cs, esp, edi, ebp;
//   void *cr3;
//   int irq;
// };

struct Context {
  void* cr3;
  uintptr_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  int irq;
  uintptr_t eip, cs;
  union {
    uint32_t val;
    struct {
      uint32_t CF :1;
      unsigned    :1;
      uint32_t PF :1;
      unsigned    :1;
      uint32_t AF :1;
      unsigned    :1;
      uint32_t ZF :1;
      uint32_t SF :1;
      uint32_t TF :1;
      uint32_t IF :1;
      uint32_t DF :1;
      uint32_t OF :1;
      unsigned    :20;
    };
  } eflags;
};

#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax

#endif
