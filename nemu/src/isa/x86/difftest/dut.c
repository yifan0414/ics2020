#include <isa.h>
#include <monitor/difftest.h>
#include "../local-include/reg.h"
#include "difftest.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  if (ref_r->pc == cpu.pc &&
      ref_r->eax == cpu.eax &&
      ref_r->ecx == cpu.ecx &&
      ref_r->edx == cpu.edx &&
      ref_r->ebx == cpu.ebx &&
      ref_r->esp == cpu.esp &&
      ref_r->ebp == cpu.ebp &&
      ref_r->esi == cpu.esi &&
      ref_r->edi == cpu.edi ) {
      return true;
  } else {
    return false;
  }
}

void isa_difftest_attach() {
}
