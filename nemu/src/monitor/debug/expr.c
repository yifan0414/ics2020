#include <isa.h>
#include <memory/paddr.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, 
  TK_EQ,     // 257
  TK_NUM,    // 258 十进制数？
  TK_REG,    // 259 寄存器？
  TK_SYMB,   // 260 符号
  TK_HEX,  	 // 261 十六进制
  TK_DEREF,  // 解引用
  TK_MINUS,  // 负数
  TK_NE,		 // !=
  TK_LE,     // <=
  TK_GE,     // >=
  TK_NOT,    // !
  TK_AND,    // &&
  TK_OR,     // ||
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},   // spaces
	{"\\+", '+'},        // \\+ => \+, regex元字符为 +
	{"\\-", '-'},        // \\- => \-, regex元字符为 -
	{"\\*", '*'},        // \\* => \*, regex元字符为 *
	{"\\/", '/'},        // \\/ => \/, regex元字符为 /
	{"\\(", '('},
	{"\\)", ')'},
  {">=", TK_GE},
  {"<=", TK_LE},
  {"<", '<'},
  {">", '>'},
  {"==", TK_EQ},        // equal
  {"!=", TK_NE},
  {"!", TK_NOT},
  {"&&", TK_AND},
  {"\\|\\|", TK_OR},
	{"0x[a-f0-9]+", TK_HEX},
	{"[0-9]+", TK_NUM},
	{"[a-zA-Z\\_][0-9a-zA-Z\\_]*", TK_SYMB},
	{"\\$(eax|ecx|edx|ebx|esp|ebp|esi|edi|eip)", TK_REG},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[65535] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
	// 这里有个bug，只把nr_token清零，但tokens内的数据依然保留，并有可能影响下面的调试命令
	// 所以应该把整个tokens结构体清零
	// 如果第一次 tokens[0].str = "10000";
	// 第二次为 tokens[0].str = "1"; 则输出时仍然为 "10000"
	// memset(tokens, 0, sizeof(tokens));
	// 第二种方法手动在字符串结尾加上 '\0' tokens[nr_token].str[substr_len] = '\0';

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        if (rules[i].token_type == TK_NOTYPE)
          break;
        Assert((substr_len < 32), "缓冲区溢出");
        strncpy(tokens[nr_token].str, substr_start, substr_len);        
        tokens[nr_token].str[substr_len] = '\0';
        tokens[nr_token].type = rules[i].token_type;
        nr_token++;
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parentheses(int p, int q, bool *success) {
	// 检查一个表达式是否被括号包围
	// 1. 首先检查括号是否合法
	// 2. 然后检查是否被括号包围
	// 3. 如果是多重括号怎么办？
	// (((1 + 1) * 10 - (5 * 2)))
	int l = 0, r = 0;
	for (int i = p; i <= q; i++) {
		if (tokens[i].type == '(') l++;
		if (tokens[i].type == ')') r++;
		if (r > l) {
			// 此时证明表达式不合法
      *success = false;
			return true;
		}
	}
  // 仍然有可能是以下情况
  // (((1+2+3)
  // bad expression
	if (l != r) {
    *success = false;
		return true;
	}
  // 以下情况都是合法表达式, 但不一定被括号包围
  Assert(l == r, "括号不匹配"); 
  *success = true;
  // 1 + (2 * 3) + 4
	if (tokens[p].type != '(' || tokens[q].type != ')')
		return false;
  // 如何排除两边都有括号，但不是相匹配的括号，如：(1+1)*(1+1)
  // 如果提前 r == l 证明提前括号闭环
  // 该情况合法
  l = 0, r = 0;
	for (int i = p; i < q; i++) {
		if (tokens[i].type == '(') l++;
		if (tokens[i].type == ')') r++;
		if (r == l) {
			return false;
		}
  }
	return true;
}


// 非运算符的token不是主运算符.
// 出现在一对括号中的token不是主运算符. 
// 主运算符的优先级在表达式中是最低的. 这是因为主运算符是最后一步才进行的运算符.
// 当有多个运算符的优先级都是最低时, 根据结合性, 最后被结合的运算符才是主运算符. 一个例子是1 + 2 + 3, 它的主运算符应该是右边的+.
int find_dominant(int p, int q, bool* success)
{

	int flag_OR = -1;
	int flag_AND = -1;
	int flag_NE_EQ = -1;
  int flag_LE_GE = -1;
  int flag_L_G = -1;
	int flag_mul_div = -1;
	int flag_add_sub = -1;
	int flag_ref = -1;
  int flag_not = -1;
  int flag_minus = -1;
	int l = 0;
	for (int i = p; i <= q; i++) {
		while (i < q && tokens[i].type == '(') {
			l++;
			i++;
		}
		while (i < q && l != 0) {
			if (tokens[i].type == '(') l++;
			if (tokens[i].type == ')') l--;
			i++;
		}
		assert(i != q + 1);
		// assert(tokens[i].type != ')');
		flag_OR = tokens[i].type == TK_OR? i : flag_OR;
		flag_AND = tokens[i].type == TK_AND? i : flag_AND;
		flag_NE_EQ = (tokens[i].type == TK_NE || tokens[i].type == TK_EQ)? i : flag_NE_EQ;
    flag_LE_GE = (tokens[i].type == TK_LE || tokens[i].type == TK_GE)? i : flag_LE_GE; 
    flag_L_G = (tokens[i].type == '>' || tokens[i].type == '<')? i : flag_L_G; 
		flag_mul_div = (tokens[i].type == '*' || tokens[i].type == '/')? i : flag_mul_div;
		flag_add_sub = (tokens[i].type == '+' || tokens[i].type == '-')? i : flag_add_sub;
		flag_ref = tokens[i].type == TK_DEREF? i : flag_ref;
    flag_minus = tokens[i].type == TK_MINUS? i : flag_minus;
    flag_not = tokens[i].type == TK_NOT? i : flag_not;
	}
	if (flag_OR != -1) {
		return flag_OR;
	} else if (flag_AND != -1) {
		return flag_AND;
	} else if (flag_NE_EQ != -1) {
		return flag_NE_EQ;
	} else if (flag_LE_GE != -1) {
    return flag_LE_GE;
  } else if (flag_L_G != -1) {
    return flag_L_G;
  } else if (flag_add_sub != -1) {
		return flag_add_sub;
	} else if (flag_mul_div != -1) {
		return flag_mul_div;
	} else if (flag_ref != -1) {
		return flag_ref;
	} else if (flag_minus != -1) {
    return flag_minus;
  } else if (flag_not != -1) {
    return flag_not;
  }
	else {
    Log("未找到主运算符");
    *success = false;
		return 0;
	}
}


word_t eval(int p, int q, bool *success) {
  if (*success == false) return 0;
  if (p > q) {
    /* Bad expression */
    *success = false;
    Log("Bad Expression!");
    return 0;
  }
  else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
		// 0x30000
		if (tokens[p].type == TK_HEX) {
			return (uint32_t) strtol(tokens[p].str, NULL, 16);
		}
		// 30000
		else if (tokens[p].type == TK_NUM) {
			// printf("num: %d\n", (uint32_t)strtol(tokens[p].str, NULL, 10));
			return (uint32_t) strtol(tokens[p].str, NULL, 10);
		}
    else if (tokens[p].type == TK_REG) {
      // 这里似乎有点凌乱，因为不能直接 #include "reg.h"
      uint32_t retval = isa_reg_str2val(tokens[p].str + 1, success);
      if (*success == false) {
        Log("寄存器值获取失败");
        return 0;
      }
      return retval;
    }
		else {
      Log("表达式未实现！");
      *success = false;
			return 0;
		}
  }
  else if (check_parentheses(p, q, success) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    if (*success == false) {
      Log("不合法的表达式: 括号不匹配");
      return 0;
    } 
    return eval(p + 1, q - 1, success);
  }
  else {
    int op = find_dominant(p, q, success);
		// if (tokens[op].type == TK_DEREF) {
		// 	int address_tmp = eval(op + 1, q, success);
		// 	return vaddr_read(address_tmp, SREG_CS, 4); // 4 个字节？
		// }
    if (tokens[op].type == TK_NOT) {
      return !eval(op + 1, q, success);
    } else if (tokens[op].type == TK_MINUS) {
      return -eval(op + 1, q, success);
    } else if (tokens[op].type == TK_DEREF) {
      return paddr_read(eval(op + 1, q, success), 4);
    }
		// printf("主运算符号为：%d\n", tokens[op].type );
		int val1 = eval(p, op - 1, success);
		int val2 = eval(op + 1, q, success);
		// printf("val1:%d val2:%d\n", val1, val2);
		switch(tokens[op].type) {
			case '+': return val1 + val2;
			case '-': return val1 - val2;
			case '*': return val1 * val2;
			case '/': /* assert(val2 != 0); */
        if (val2 == 0) {
          *success = false;
          // printf(RED"表达式含除0操作！\n"RESET);
          Log("表达式含除0操作!");
          return 0;
        }
        return val1 / val2;
      case '>': return val1 > val2;
      case '<': return val1 < val2;
			case TK_EQ : return val1 == val2;
			case TK_NE : return val1 != val2;
      case TK_LE : return val1 <= val2;
      case TK_GE : return val1 >= val2;
			case TK_AND: return val1 && val2;
			case TK_OR : return val1 || val2;
			default: *success = false; return 0;
		}
    return 0;
  }
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  // TODO();
  for(int i = 0; i < nr_token; i ++) {
      if(tokens[i].type == '-' && (i == 0 || tokens[i - 1].type == TK_AND || 
        tokens[i - 1].type == TK_OR || tokens[i - 1].type == TK_EQ ||
        tokens[i - 1].type == TK_NE || tokens[i - 1].type == TK_NOT ||
        tokens[i - 1].type == TK_LE || tokens[i - 1].type == TK_GE ||
        tokens[i - 1].type == '>' || tokens[i - 1].type == '<' ||
        tokens[i - 1].type == '+' || tokens[i - 1].type == '-' ||
        tokens[i - 1].type == '*' || tokens[i - 1].type == '/'
        ) ) {
          tokens[i].type = TK_MINUS;
      }
  }
  for(int i = 0; i < nr_token; i ++) {
      if(tokens[i].type == '*' && (i == 0 || tokens[i - 1].type == TK_AND || 
        tokens[i - 1].type == TK_OR || tokens[i - 1].type == TK_EQ ||
        tokens[i - 1].type == TK_NE || tokens[i - 1].type == TK_NOT ||
        tokens[i - 1].type == TK_LE || tokens[i - 1].type == TK_GE ||
        tokens[i - 1].type == '>' || tokens[i - 1].type == '<' ||
        tokens[i - 1].type == '+' || tokens[i - 1].type == '-' ||
        tokens[i - 1].type == '*' || tokens[i - 1].type == '/'
        ) ) {
          tokens[i].type = TK_DEREF;
      }
  }
  // 打印 tokens
  // for (int i = 0; i < nr_token; i++) {
  //   printf("type: %d %s\n", tokens[i].type, tokens[i].str);
  // }
  return eval(0, nr_token - 1, success);
}
