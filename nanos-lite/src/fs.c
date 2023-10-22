#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum { FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB };

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
    [FD_STDIN] = {"stdin", 0, 0, invalid_read, invalid_write},
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

// return the file_descriptor
int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < sizeof(file_table) / sizeof(file_table[0]); i++) {
    if (strcmp(pathname, file_table[i].name) == 0) {
      file_table[i].open_offset = 0;
      return i;
    }
  }
  assert(0);
}

size_t fs_lseek(int fd, size_t offset, int whence) {
  if (whence == SEEK_SET) {
    file_table[fd].open_offset = offset;
  } else if (whence == SEEK_CUR) {
    file_table[fd].open_offset += offset;
  } else if (whence == SEEK_END) {
    file_table[fd].open_offset = file_table[fd].size + offset;
  } else {
    assert(0);
  }
  assert(file_table[fd].open_offset >= 0 &&
         file_table[fd].open_offset <= file_table[fd].size);
  // printf("lseek = %d\n", file_table[fd].open_offset);
  return file_table[fd].open_offset;
}

size_t fs_read(int fd, void *buf, size_t len) {
  printf("fd.name = %s\n", file_table[fd].name);
  printf("len = %d\n", len);
  printf("open_offset= %d\n", file_table[fd].open_offset);
  assert(file_table[fd].open_offset + len <= file_table[fd].size);
  ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset,
               len);
  file_table[fd].open_offset += len;
  return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  if (fd == 0 || fd == 1 || fd == 2) {
    char *p = (char *)buf;
    for (int i = 0; i < len; i++) {
      putch(p[i]);
    }
    return len;
  }
  assert(file_table[fd].open_offset + len <= file_table[fd].size);
  ramdisk_write(buf, file_table[fd].disk_offset + file_table[fd].open_offset,
                len);
  file_table[fd].open_offset += len;
  return len;
}

int fs_close(int fd) { return 0; }

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
