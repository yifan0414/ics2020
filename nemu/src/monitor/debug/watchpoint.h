#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>


typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  bool in_use;
  char* expr;
  uint32_t val;

  /* TODO: Add more members if necessary */

} WP;

void free_wp(WP *wp);
WP* new_wp();
void list_wp();
// int scan_wp();
WP* get_wp();


#endif
