#include "trap.h"

int main() {
	__asm__ volatile ("movl $0x01, %%eax\n\t"
    "movl $0x02, %%ebx\n\t"
    "and %%ebx,%%eax\n\t"
    ".byte 0x6a,0xff\n\t"
		:
		:
		:
		);
	return 0;
}

