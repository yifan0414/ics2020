#include <isa.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  if (args == NULL) {
    cpu_exec(1);
    return 0;
  }
  char *steps = strtok(NULL, " ");
  // Assert(atoi(steps), "非法输入"); // May be too dangerous
  int n = atoi(steps);
  if (n == 0) {
    Log("illegal arguments!");
    return 0;
  }
  cpu_exec(n);
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) {
    // 这里到底是直接退出好呢? 还是重来?
    // Assert(0, "缺少参数");
    Log("Please give one args from 'r' and 'w'");
    return 0;
  }
  // 看起来多此一举, 但是其作用是忽略所有的空白字符
  char* arg = strtok(NULL, " ");
  if (strcmp(arg, "r") == 0) {
    isa_reg_display();
  }
  else if (strcmp(arg, "w") == 0) {
    // TODO: 实现打印监视点信息
    TODO();
  }
  else {
    Log("undefined info args");
  } 
  return 0;
}

static int cmd_x(char *args) {
  TODO();
}

static int cmd_p(char *args) {
  if (args == NULL) {
    Log("Please give me a expression");
    return 0;
  }
  // 如果 args = "   1   + 1" 那么 arg = "1". 显然不符合要求
  // char *arg = strtok(NULL, " ");
  bool success = true;
  word_t val = expr(args, &success);
  if (success == true)
    printf("0x%08x\t%u\n", val, val);
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Single Step Execution", cmd_si},
  { "info", "Print register and watch point info", cmd_info},
  { "x", "Scan the memory", cmd_x},
  { "p", "Evaluate the expression", cmd_p},


  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s\t - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s\t - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1; // 如果不仅仅是一个空格呢?
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
