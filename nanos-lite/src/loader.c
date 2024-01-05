#include <elf.h>
#include <fs.h>
#include <proc.h>

extern uint8_t ramdisk_start;
extern uint8_t ramdisk_end;

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#endif

static uintptr_t loader(PCB *pcb, const char *filename) {
  // 思考误区
  int fd = fs_open(filename, 0, 0);
  Elf_Ehdr elf;
  fs_read(fd, &elf, sizeof(Elf_Ehdr));
  // 判定是否是ELF文件
  assert(elf.e_ident[0] == 0x7f && elf.e_ident[1] == 0x45 &&
         elf.e_ident[2] == 0x4c && elf.e_ident[3] == 0x46);
  fs_lseek(fd, elf.e_phoff, SEEK_SET);
  Elf_Phdr ph[elf.e_phnum];
  for (int i = 0; i < elf.e_phnum; i++) {
    // READ from each program header
    fs_read(fd, &ph[i], elf.e_phentsize);
  }
  for (int i = 0; i < elf.e_phnum; i++) {
    if (ph[i].p_type == PT_LOAD) {
      fs_lseek(fd, ph[i].p_offset, SEEK_SET);
      fs_read(fd, (void *)ph[i].p_vaddr, ph[i].p_filesz);
      if (ph[i].p_memsz > ph[i].p_filesz) {
        memset((void *)(ph[i].p_vaddr + ph[i].p_filesz), 0,
               ph[i].p_memsz - ph[i].p_filesz);
      }
    }
  }
  return (uintptr_t)elf.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename); // 运行时装入
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}
