#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0, canvas_w = 0, canvas_h = 0;
static uint32_t* canvas;

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
  } else {
    if (*w == 0 && *h == 0) *w = screen_w, *h = screen_h;
    canvas_w = *w; canvas_h = *h;
    assert(canvas_w <= screen_w && canvas_h <= screen_h);
    canvas = malloc(canvas_w * canvas_h * sizeof(uint32_t));
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  if (x == 0 && y == 0 && w == 0 && h == 0) {
    w = canvas_w, h = canvas_h;
  }
  FILE* fb_file = fopen("/dev/fb", "w");
  uint32_t tmp0 = 0;
  int h_offset = (screen_h - canvas_h) / 2, 
      w_offset = (screen_w - canvas_w) / 2;
  for (int i = -h_offset; i < screen_h - h_offset; i++)
    for (int j = -w_offset; j < screen_w - w_offset; j++) {
      // if (i >= 0 && j >= 0 && i < canvas_h && j < canvas_w) {
        fwrite(&pixels[i * canvas_w + j], sizeof(uint32_t), 1, fb_file);
      // } else {
      //   fwrite(&tmp0, sizeof(uint32_t), 1, fb_file);
      // }
    }
  fclose(fb_file);
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
  FILE* dispinfo_file = fopen("/proc/dispinfo", "r");
  fscanf(dispinfo_file, "WIDTH : %d\nHEIGHT:%d", &screen_w, &screen_h);
  printf("screen_w : %d, screen_h : %d\n", screen_w, screen_h);
  printf("START\n");
  return 0;
}

void NDL_Quit() {
}
