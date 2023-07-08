#include "trap.h"

int main() {
	__asm__ volatile ("movl $0x01, %%eax\n\t"
    "movl $0x02, %%ebx\n\t"
    "addl $0x01, %%eax\n\t"
    "\n\t"
		:
		:
		:
		);
	return 0;
}

