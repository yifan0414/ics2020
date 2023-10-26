#include <fs.h>

typedef size_t (*ReadFn)(void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn)(const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);

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
    [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
    [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
    {"/dev/events", 0, 0, events_read, invalid_write},
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
  return file_table[fd].open_offset;
}

size_t fs_read(int fd, void *buf, size_t len) {
  // 尽可能的读取
  if (file_table[fd].size &&
      file_table[fd].open_offset + len > file_table[fd].size) {
    if (file_table[fd].size < file_table[fd].open_offset)
      assert(0);
    else
      len = file_table[fd].size - file_table[fd].open_offset;
  }
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
  size_t ret = 0;
  if (file_table[fd].read) {
    ret = file_table[fd].read(buf, offset, len);
  } else {
    ret = ramdisk_read(buf, offset, len);
  }
  file_table[fd].open_offset += ret;
  return ret;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  assert(file_table[fd].open_offset <= file_table[fd].size);
  if (file_table[fd].size &&
      file_table[fd].open_offset + len > file_table[fd].size) {
    if (file_table[fd].size && file_table[fd].size < file_table[fd].open_offset)
      assert(0);
    else
      len = file_table[fd].size - file_table[fd].open_offset;
  }
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
  size_t ret = 0;

  if (file_table[fd].write) {
    ret = file_table[fd].write(buf, offset, len);
  } else {
    ret = ramdisk_write(buf, offset, len);
  }
  if (fd != 0 && fd != 1 && fd != 2) {
    file_table[fd].open_offset += ret;
  }
  return ret;
}

int fs_close(int fd) { return 0; }

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
