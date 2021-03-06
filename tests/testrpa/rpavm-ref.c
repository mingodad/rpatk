#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include "rlib/rmem.h"
#include "rpa/rpacompiler.h"
#include "rpa/rpastatpriv.h"
#include "common.h"



void code_rpa_matchabc(rpa_compiler_t *co, rpastat_t *stat)
{
	unsigned long ruleidx;
	const char *rule = "rpa_matchabc";
	const char *ruleend = "rpa_matchabc_end";

	ruleidx = rvm_codegen_addstring_s(co->cg, NULL, rule);
	rvm_codegen_addlabel_default_s(co->cg, rule);

	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_STRING, ruleidx, rvm_asm(RPA_EMITSTART, DA, R_TOP, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSHM, DA, XX, XX, BIT(R_TOP)|BIT(LR)));

	rvm_codegen_addins(co->cg, rvm_asm(RPA_MATCHCHR_NAN, DA, XX, XX, 'a'));
	rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, ruleend, rvm_asm(RVM_BLES, DA, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RPA_MATCHCHR_NAN, DA, XX, XX, 'b'));
	rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, ruleend, rvm_asm(RVM_BLES, DA, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RPA_MATCHCHR_NAN, DA, XX, XX, 'c'));
	rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, ruleend, rvm_asm(RVM_BLES, DA, XX, XX, 0));

	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R1)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_SUBS, R0, R_TOP, R1, 0));
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_STRING, ruleidx, rvm_asm(RPA_EMITEND, DA, R1, R0, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_addlabel_default_s(co->cg, ruleend);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
}


void code_rpa_matchxyz(rpa_compiler_t *co, rpastat_t *stat)
{
	unsigned long ruleidx;
	const char *rule = "rpa_matchxyz";
	const char *ruleend = "rpa_matchxyz_end";

	ruleidx = rvm_codegen_addstring_s(co->cg, NULL, rule);
	rvm_codegen_addlabel_default_s(co->cg, rule);

	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_STRING, ruleidx, rvm_asm(RPA_EMITSTART, DA, R_TOP, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSHM, DA, XX, XX, BIT(R_TOP)|BIT(LR)));

	rvm_codegen_addins(co->cg, rvm_asm(RPA_MATCHCHR_NAN, DA, XX, XX, 'x'));
	rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, ruleend, rvm_asm(RVM_BLES, DA, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RPA_MATCHCHR_NAN, DA, XX, XX, 'y'));
	rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, ruleend, rvm_asm(RVM_BLES, DA, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RPA_MATCHCHR_NAN, DA, XX, XX, 'z'));
	rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, ruleend, rvm_asm(RVM_BLES, DA, XX, XX, 0));

	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R1)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_SUBS, R0, R_TOP, R1, 0));
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_STRING, ruleidx, rvm_asm(RPA_EMITEND, DA, R1, R0, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_addlabel_default_s(co->cg, ruleend);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
}

void code_rpa_matchmnode(rpa_compiler_t *co, rpastat_t *stat)
{
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R_TOP, DA, XX, 0));
	rpa_compiler_reference_nan_s(co, "rpa_matchabc");
	VMTEST_REG(co->cg, 0, 3, "RPA_REFERENCE_NAN 'abc'");
	VMTEST_STATUS(co->cg, 0, "RPA_REFERENCE_NAN STATUS");

	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R_TOP, DA, XX, 0));
	rpa_compiler_reference_nan_s(co, "rpa_matchxyz");
	VMTEST_REG(co->cg, 0, -1, "RPA_REFERENCE_NAN 'xyz'");
	VMTEST_STATUS(co->cg, RVM_STATUS_N, "RPA_REFERENCE_NAN STATUS");

	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R_TOP, DA, XX, 0));
	rpa_compiler_reference_opt_s(co, "rpa_matchabc");
	VMTEST_REG(co->cg, 0, 3, "RPA_REFERENCE_OPT 'abc'");
	VMTEST_STATUS(co->cg, 0, "RPA_REFERENCE_OPT STATUS");

	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R_TOP, DA, XX, 0));
	rpa_compiler_reference_opt_s(co, "rpa_matchxyz");
	VMTEST_REG(co->cg, 0, 0, "RPA_REFERENCE_OPT 'xyz'");
	VMTEST_STATUS(co->cg, RVM_STATUS_Z, "RPA_REFERENCE_OPT STATUS");

	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R_TOP, DA, XX, 0));
	rpa_compiler_reference_mul_s(co, "rpa_matchabc");
	VMTEST_REG(co->cg, 0, 12, "RPA_REFERENCE_MUL 'abc'");
	VMTEST_STATUS(co->cg, 0, "RPA_REFERENCE_MUL STATUS");

	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R_TOP, DA, XX, 0));
	rpa_compiler_reference_mul_s(co, "rpa_matchxyz");
	VMTEST_REG(co->cg, 0, -1, "RPA_REFERENCE_MUL 'xyz'");
	VMTEST_STATUS(co->cg, RVM_STATUS_N, "RPA_REFERENCE_MUL STATUS");

	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R_TOP, DA, XX, 0));
	rpa_compiler_reference_mop_s(co, "rpa_matchabc");
	VMTEST_REG(co->cg, 0, 12, "RPA_REFERENCE_MOP 'abc'");
	VMTEST_STATUS(co->cg, 0, "RPA_REFERENCE_MOP STATUS");

	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R_TOP, DA, XX, 0));
	rpa_compiler_reference_mop_s(co, "rpa_matchxyz");
	VMTEST_REG(co->cg, 0, 0, "RPA_REFERENCE_MOP 'xyz'");
	VMTEST_STATUS(co->cg, RVM_STATUS_Z, "RPA_REFERENCE_MOP STATUS");
}


int main(int argc, char *argv[])
{
	rvm_codelabel_t *err;
	rpa_compiler_t *co;
	rpastat_t *stat;
	unsigned int mainoff;
	char teststr[] = "abcabcabcabc";

	co = rpa_compiler_create();
	stat = rpa_stat_create(NULL, 4096);
	rvm_cpu_addswitable(stat->cpu, "common_table", common_calltable);

	rpa_stat_init(stat, RPA_ENCODING_UTF8, teststr, teststr, teststr+12, NULL);

	mainoff = rvm_codegen_addins(co->cg, rvm_asml(RVM_NOP, XX, XX, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_MOV, R_TOP, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_MOV, FP, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_MOV, SP, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RPA_SHIFT, XX, XX, XX, 0));

	code_rpa_matchmnode(co, stat);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_NOP, XX, XX, XX, 0xabc));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_EXT, XX, XX, XX, 0));
	code_rpa_matchabc(co, stat);
	code_rpa_matchxyz(co, stat);

	if (rvm_codegen_relocate(co->cg, &err) < 0) {
		r_printf("Unresolved symbol: %s\n", err->name->str);
		goto end;
	}

	rvm_cpu_exec(stat->cpu, rvm_codegen_getcode(co->cg, 0), mainoff);

end:
	rpa_stat_destroy(stat);
	rpa_compiler_destroy(co);


	r_printf("Max alloc mem: %ld\n", r_debug_get_maxmem());
	r_printf("Leaked mem: %ld\n", r_debug_get_allocmem());
	return 0;
}
