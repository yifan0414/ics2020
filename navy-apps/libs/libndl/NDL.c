#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

int _read(int fd, void *buf, size_t count);

uint32_t NDL_GetTicks() {
  struct timeval time;
  gettimeofday(&time, NULL);
  return time.tv_sec * 1000 + time.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  FILE* evt_file = fopen("/dev/events", "r");
  char ch = -1;
  int i = 0;
  // while (i < len && (ch = fgetc(evt_file)) != EOF) {
  //   buf[i++] = ch;
  // }
  i = fread(buf, sizeof(char), len, evt_file); 
  // 如果将这个注释, 那么将不会读取 "dev/events" 的内容, 并且也不会有窗口生成.
  fclose(evt_file);
  return i ? 1 : 0;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  printf("START\n");
  return 0;
}

void NDL_Quit() {
}
