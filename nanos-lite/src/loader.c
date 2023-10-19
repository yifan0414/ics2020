#include <proc.h>
#include <elf.h>

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;
size_t ramdisk_read(void *buf, size_t offset, size_t len);
#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr* elf = (Elf_Ehdr*) &ramdisk_start;
  Elf_Phdr* ph  = (Elf_Phdr*) ((void*)elf + elf->e_phoff);
  Elf_Phdr* eph = (Elf_Phdr*) (ph + elf->e_phnum);
  assert(*(uint32_t *)elf->e_ident == 0x464c457f);
  // printf("elf = %p, ph = %p, eph = %p margic = 0x%x\n",elf, elf->e_phoff, elf->e_phnum, *(uint32_t *)elf->e_ident);
  for (; ph < eph; ph++) {
    // printf("ph->p_type = %d\n", ph->p_type);
    if (ph->p_type == PT_LOAD) {
      ramdisk_read((void *)ph->p_vaddr, ph->p_offset, ph->p_filesz);
      if (ph->p_memsz > ph->p_filesz) {
        memset((void *)(ph->p_vaddr + ph->p_filesz), 0, ph->p_memsz - ph->p_filesz);
      }
    }
  }
  volatile uintptr_t entry = elf->e_entry;
  return entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

