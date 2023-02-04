#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
    wp_pool[i].in_use = false;
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp() {
  assert(free_ != NULL);
  WP *wp = free_;
  free_ = free_->next;
  assert(wp->in_use == false);
  wp->in_use = true;
  wp->next = head;
  head = wp;
  return wp;
}

void free_wp(WP *wp) {
  assert(wp >= wp_pool && wp < wp_pool + NR_WP);
  assert(wp->in_use == true);
  wp->in_use = false;
  wp->next = free_;
  free_ = wp;
}

void list_wp() {
  WP* wp = head;
  printf("Num\tWhat\tValue\n");
  while (wp != NULL) {
    printf("%d\t%s\t0x%x\n", wp->NO, wp->expr, wp->val);
    wp = wp->next;
  }
}

// int scan_wp() {
//   bool flag = false;
//   WP* wp;
//   for (wp = head; wp != NULL; wp = wp->next) {
//     assert(wp->in_use == true);
//     bool success = true;
//     uint32_t new_val = expr(wp->expr, &success);
//     if (new_val != wp->val) {
//       printf("")
//     }
//     flag = true;
//   }
// }

WP* get_wp() {
  return head;
}