static inline def_EHelper(mov) {
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(mov);
}

static inline def_EHelper(push) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_push(s, ddest);
  print_asm_template1(push);
}

static inline def_EHelper(pop) {
  rtl_pop(s, ddest);
  operand_write(s, id_dest, ddest);
  print_asm_template1(pop);
}

static inline def_EHelper(pusha) {
  TODO();
  print_asm("pusha");
}

static inline def_EHelper(popa) {
  TODO();
  print_asm("popa");
}

static inline def_EHelper(leave) {
  rtl_mv(s, &cpu.esp, &cpu.ebp);
  rtl_pop(s, &cpu.ebp);
  print_asm("leave");
}

static inline def_EHelper(cltd) {
  if (s->isa.is_operand_size_16) {
    rtl_lr(s, s0, R_AX, 2);
    if (*s0 & 0x8000) {
      *s0 = 0xffff;
    } else {
      *s0 = 0;
    }
    rtl_sr(s, R_DX, s0, 2);
  }
  else {
    rtl_lr(s, s0, R_EAX, 4);
    if (*s0 & 0x80000000) {
      *s0 = 0xffffffff;
    } else {
      *s0 = 0;
    }
    rtl_sr(s, R_EDX, s0, 4);
  }
  print_asm(s->isa.is_operand_size_16 ? "cwtl" : "cltd");
}

// NO TEST
static inline def_EHelper(cwtl) {
  // if (s->isa.is_operand_size_16) {
  //   cpu.gpr[0]._16 = cpu.gpr[0]._8[0];
  // }
  // else {
  //   cpu.gpr[0]._32 = cpu.gpr[0]._16;
  // }
  if (s->isa.is_operand_size_16) {
    rtl_lr(s, s0, R_AL, 1);
    rtl_sext(s, s0, s0, 1);
    rtl_sr(s, R_AX, s0, 2);
  }
  else {
    rtl_lr(s, s0, R_AX, 2);
    rtl_sext(s, s0, s0, 2);
    rtl_sr(s, R_EAX, s0, 4);
  }
  print_asm(s->isa.is_operand_size_16 ? "cbtw" : "cwtl");
}

static inline def_EHelper(movsx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  rtl_sext(s, ddest, dsrc1, id_src1->width);
  operand_write(s, id_dest, ddest);
  print_asm_template2(movsx);
}

static inline def_EHelper(movzx) {
  id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
  operand_write(s, id_dest, dsrc1);
  print_asm_template2(movzx);
}


static inline def_EHelper(movsb) {
  operand_write(s, id_dest, dsrc1);
  if (cpu.eflags.DF == 0) {
    cpu.esi += id_dest->width;
    cpu.edi += id_dest->width;
  } else {
    cpu.esi -= id_dest->width;
    cpu.edi -= id_dest->width;
  }
  // print_asm_template1(movsb);
}

static inline def_EHelper(lea) {
  rtl_addi(s, ddest, s->isa.mbase, s->isa.moff);
  operand_write(s, id_dest, ddest);
  print_asm_template2(lea);
}

static inline def_EHelper(stos) {
  operand_write(s, id_dest, dsrc1);
  if (cpu.eflags.DF == 0) {
    cpu.edi += id_dest->width;
  } else {
    cpu.edi -= id_dest->width;
  }
  // print_asm_template1(stosb);
}

// NO TEST
// static inline def_EHelper(bsr) {
//   id_dest->width = s->isa.is_operand_size_16 ? 2 : 4;
//   rtl_li(s, s1, 1);
//   int position = -1;
//   if (id_dest->width == 4) {
//     for (int i = 31; i != 0; i--) {
//       if (*dsrc1 & (1 << i)) {
//         position = i;
//         break;
//       }
//     }
//     if (position == -1) {
//       rtl_set_ZF(s, s1);
//     } else {
//       rtl_set_ZF(s, rz);
//       rtl_li(s, ddest, position);
//     }
//   } else {
//     for (int i = 31; i != 0; i--) {
//       if (*dsrc1 & (1 << i)) {
//         position = i;
//         break;
//       }
//     }
//     if (position == -1) {
//       rtl_set_ZF(s, s1);
//     } else {
//       rtl_set_ZF(s, rz);
//       rtl_li(s, ddest, position);
//     }
//   }
//   print_asm_template2(bsr);
// }