#include <stdio.h>
#include "common.h"
#include "rvmreg.h"

	
int main(int argc, char *argv[])
{
	ruint ret = 0;
	ruint off = 0;
	rvmreg_t d1 = rvm_reg_create_double(-2.0);
	rvmreg_t d1s = rvm_reg_create_string_ansi("-2.0");
	rvmreg_t d2 = rvm_reg_create_double(3.0);
	rvmreg_t d3 = rvm_reg_create_double(-3.0);

	rvm_asmins_t vmcode[256];
	rvmcpu_t *vm = rvm_cpu_create_default();
	
	rvm_cpu_addswitable(vm, "common_table", common_calltable);

	vmcode[off++] = rvm_asm(RVM_MOV, R0, DA, XX, 1);
	vmcode[off++] = rvm_asmp(RVM_LDRR, R1, DA, XX, &d1s);
	vmcode[off++] = rvm_asm(RVM_ESUB, R0, R0, R1, 0);
	vmcode[off++] = rvm_asm(RVM_PRN, R0, XX, XX, 0);
	VMTEST_REGD(vmcode, off, 0, 3.0, "1:ESUB");
	VMTEST_STATUS(vmcode, off, 0, "1:ESUB STATUS");

	vmcode[off++] = rvm_asm(RVM_MOV, R0, DA, XX, 1);
	vmcode[off++] = rvm_asmp(RVM_LDRR, R1, DA, XX, &d1s);
	vmcode[off++] = rvm_asm(RVM_ESUB, R0, R1, R0, 0);
	VMTEST_REGD(vmcode, off, 0, -3, "2:ESUB");
	VMTEST_STATUS(vmcode, off, RVM_STATUS_N, "2:ESUB STATUS");

	vmcode[off++] = rvm_asm(RVM_MOV, R0, DA, XX, 1);
	vmcode[off++] = rvm_asm(RVM_MOV, R1, DA, XX, 2);
	vmcode[off++] = rvm_asm(RVM_ESUB, R0, R1, R0, 0);
	vmcode[off++] = rvm_asm(RVM_PRN, R0, XX, XX, 0);
	VMTEST_REG(vmcode, off, 0, 1, "3:ESUB");
	VMTEST_STATUS(vmcode, off, RVM_STATUS_C, "3:ESUB STATUS");

	vmcode[off++] = rvm_asm(RVM_MOV, R0, DA, XX, 1);
	vmcode[off++] = rvm_asmp(RVM_LDRR, R1, DA, XX, &d1);
	vmcode[off++] = rvm_asm(RVM_ESUB, R0, R1, R0, 0);
	vmcode[off++] = rvm_asm(RVM_PRN, R0, XX, XX, 0);
	VMTEST_REGP(vmcode, off, 0, &d3, "4:ESUB");
	VMTEST_STATUS(vmcode, off, RVM_STATUS_N|RVM_STATUS_C, "4:ESUB STATUS");

	vmcode[off++] = rvm_asm(RVM_MOV, R0, DA, XX, 1);
	vmcode[off++] = rvm_asmp(RVM_LDRR, R1, DA, XX, &d1);
	vmcode[off++] = rvm_asm(RVM_ESUB, R0, R0, R1, 0);
	vmcode[off++] = rvm_asm(RVM_PRN, R0, XX, XX, 0);
	VMTEST_REGP(vmcode, off, 0, &d2, "5:ESUB");
	VMTEST_STATUS(vmcode, off, RVM_STATUS_C, "5:ESUB STATUS");

	vmcode[off++] = rvm_asm(RVM_MOV, R0, DA, XX, 1);
	vmcode[off++] = rvm_asml(RVM_MOV, R1, DA, XX, 2);
	vmcode[off++] = rvm_asm(RVM_ESUB, R0, R1, R0, 0);
	vmcode[off++] = rvm_asm(RVM_PRN, R0, XX, XX, 0);
	VMTEST_REGL(vmcode, off, 0, 1, "6:ESUB");
	VMTEST_STATUS(vmcode, off, RVM_STATUS_C, "6:ESUB STATUS");

	vmcode[off++] = rvm_asm(RVM_EXT, R0, XX, XX, 0);
#ifdef EXECDEBUG
	ret = rvm_cpu_exec_debug(vm, vmcode, 0);
#else
	ret = rvm_cpu_exec(vm, vmcode, 0);
#endif
	rvm_cpu_destroy(vm);
	rvm_reg_cleanup(&d1s);
	return 0;
}