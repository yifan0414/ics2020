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
  Elf_Phdr* eph = (Elf_Phdr*) (ph + elf->e_phnum); // 注意这里是按照数组地址加法
  assert(*(uint32_t *)elf->e_ident == 0x464c457f); // 检查是否是ELF文件
  for (; ph < eph; ph++) {
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

