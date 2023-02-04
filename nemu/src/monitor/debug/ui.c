#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include <memory/paddr.h>

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();
void asm_print(vaddr_t this_pc, int instr_len, bool print_flag);

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
    list_wp();
  }
  else {
    Log("undefined info args");
  } 
  return 0;
}

static int cmd_x(char *args) {
	if (args == NULL) {
		Log("Command Format: \"x format address\"");
		return 0;
	}
 
  bool success = true;
  char arg1[32] = {'\0'};
  char arg2[32] = {'\0'};
  if (sscanf(args, "%s %[^\n]", arg1, arg2) != 2) {
    Log("invalid expression: '%s'\n", args);
    return 0;
  }
  int length = 1;
  char format = 'x';
  char size = 'w';
  // if (sscanf(arg1, "%d%c%c", &length, &format, &size) != 3) {
  //   Log("Not conform to the format: \"x format expression\"");
  //   return 0;
  // }
  sscanf(arg1, "%d%c%c", &length, &format, &size);
  int addr = expr(arg2, &success);
  if (success == false) {
    Log("invalid expression: '%s'\n", args);
    return 0;
  }

  int single_size = 0;
  if (size == 'b') {
    single_size = 1;
  } else if (size == 'h') {
    single_size = 2;
  } else if (size == 'w') {
    single_size = 4;
  } else {
    Log("Undefined size format!");
    return 0;
  }

  // TODO: 这一段代码非常 shit，怎么提高可维护性呢？两个变量
  if (format == 'x') {
    for (int i = 0; i < (float) length / (8 / single_size); i ++) {
      printf("\x1B[34m0x%x:\x1B[0m\t", addr + i * 8);
      for (int j = 0; j + i * (8 / single_size) < length && j < (8 / single_size); j++) {
        if (single_size == 1) {
          printf("0x%02x\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        } else if (single_size == 2) {
          printf("0x%04x\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        } else if (single_size == 4) {
          printf("0x%08x\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        }
      }
      printf("\n");
    }
  } else if (format == 'd') {
    for (int i = 0; i < (float) length / (8 / single_size); i ++) {
      printf("\x1B[34m0x%x:\x1B[0m\t", addr + i * 8);
      for (int j = 0; j < length && j < (8 / single_size); j++) {
        if (single_size == 1) {
          printf("%d\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        } else if (single_size == 2) {
          printf("%d\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        } else if (single_size == 4) {
          printf("%d\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        }
      }
      printf("\n");
    }
  } else if (format == 'u') {
    for (int i = 0; i < (float) length / (8 / single_size); i ++) {
      printf("\x1B[34m0x%x:\x1B[0m\t", addr + i * 8);
      for (int j = 0; j < length && j < (8 / single_size); j++) {
        if (single_size == 1) {
          printf("%u\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        } else if (single_size == 2) {
          printf("%u\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        } else if (single_size == 4) {
          printf("%u\t", paddr_read((uint32_t) addr + i * 8 + j * single_size, single_size));
        }
      }
      printf("\n");
    }
  } else if (format == 'i') {
    // 这是模拟器，没办法提前分析汇编指令
    Log("Undefined output format \"i\"");
  } else {
    Log("Undefined output format \"%c\"", format);
  } 
  return 0;
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

static int cmd_w(char *args) {
  WP* wp = new_wp();
  wp->expr = strdup(args); // copy the args, remember free
  assert(wp->expr != NULL);
  bool success = true;
  wp->val = expr(wp->expr, &success);
  if (success == false) {
    Log("求值失败");
    return 0;
  }
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
  { "w", "set the watchpoint", cmd_w},

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
