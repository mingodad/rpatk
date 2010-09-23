#include <stdio.h>
#include <stdlib.h>
#include "rvmcpu.h"


static void test_swi_sub(rvm_cpu_t *cpu)
{
	rword res, op2 = RVM_GET_REGU(cpu, R0), op3 = RVM_GET_REGU(cpu, R1);

	res = op2 - op3;
	RVM_SET_REGU(cpu, R0, res);
}


static void test_swi_mul(rvm_cpu_t *cpu)
{
	rword res, op2 = RVM_GET_REGU(cpu, R0), op3 = RVM_GET_REGU(cpu, R1);

	res = op2 * op3;
	RVM_SET_REGU(cpu, R0, res);
}


static rvm_switable_t switable[] = {
		{"sub", test_swi_sub},
		{"mul", test_swi_mul},
		{NULL, NULL},
};


int main(int argc, char *argv[])
{
	rvm_cpu_t *cpu;
	rvm_asmins_t code[1024];
	ruint off = 0;
	ruint ntable;

	cpu = rvm_cpu_create();
	ntable = rvm_cpu_switable_add(cpu, switable);
	ntable = rvm_cpu_switable_add(cpu, switable);

	code[off++] = rvm_asm(RVM_MOV, R0, DA, XX, 1);
	code[off++] = rvm_asm(RVM_MOV, R1, DA, XX, 2);
	code[off++] = rvm_asm(RVM_ADD, R2, R0, R1, 0);
	code[off++] = rvm_asm(RVM_SWI, DA, XX, XX, RVM_SWI_ID(ntable, 1));		// mul
	code[off++] = rvm_asm(RVM_SWI, DA, XX, XX, RVM_SWI_ID(ntable, 0));		// sub
	code[off++] = rvm_asm(RVM_EXT, XX, XX, XX, 0);
	rvm_cpu_exec_debug(cpu, code, 0);
	rvm_cpu_destroy(cpu);


	fprintf(stdout, "It works!\n");
	return 0;
}
