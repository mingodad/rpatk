/*
 *  Regular Pattern Analyzer Toolkit (RPA/Tk)
 *  Copyright (c) 2009-2012 Martin Stoilov
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Martin Stoilov <martin@rpasearch.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include "rvm/rvmcodegen.h"
#include "rlib/rstring.h"
#include "rlib/rmem.h"
#include "rvm/rvmcpu.h"
#include "rvm/rvmreg.h"
#include "rvm/rvmoperator.h"


typedef struct rvm_testctx_s {
	rvm_opmap_t *opmap;
} rvm_testctx_t;


static void test_swi_cat(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rvm_testctx_t *ctx = (rvm_testctx_t *)cpu->userdata1;
	rvm_opmap_invoke_binary_handler(ctx->opmap, RVM_OPID_CAT, cpu, RVM_CPUREG_PTR(cpu, ins->op1), RVM_CPUREG_PTR(cpu, ins->op2), RVM_CPUREG_PTR(cpu, ins->op3));
}


static void test_swi_print_r(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	if (rvm_reg_gettype(RVM_CPUREG_PTR(cpu, ins->op1)) == RVM_DTYPE_SIGNED)
		fprintf(stdout, "R%d = %ld\n", ins->op1, RVM_CPUREG_GETL(cpu, ins->op1));
	else if (rvm_reg_gettype(RVM_CPUREG_PTR(cpu, ins->op1)) == RVM_DTYPE_DOUBLE)
		fprintf(stdout, "R%d = %5.2f\n", ins->op1, RVM_CPUREG_GETD(cpu, ins->op1));
	else if (rvm_reg_gettype(RVM_CPUREG_PTR(cpu, ins->op1)) == RVM_DTYPE_STRING)
		fprintf(stdout, "R%d = %s\n", ins->op1, ((rstring_t*) RVM_CPUREG_GETP(cpu, ins->op1))->s.str);
	else
		fprintf(stdout, "Unknown type\n");
}


static void test_swi_strinit(rvmcpu_t *cpu, rvm_asmins_t *ins)
{

}


static void test_swi_unref(rvmcpu_t *cpu, rvm_asmins_t *ins)
{

}


static void test_swi_strtodouble(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rstring_t *s = (rstring_t*)RVM_CPUREG_GETP(cpu, ins->op1);
	double d = strtod(s->s.str, NULL);
	RVM_CPUREG_SETD(cpu, R0, d);
}


static rvm_switable_t switable[] = {
		{"str_init", test_swi_strinit},
		{"str_to_double", test_swi_strtodouble},
		{"unref", test_swi_unref},
		{"cat", test_swi_cat},
		{"print", test_swi_print_r},
		{NULL, NULL},
};


int main(int argc, char *argv[])
{
	char *hello = "Hello World";
	char *there = ", right there";

	rvm_testctx_t ctx;
	rvm_codegen_t *cg;
	rvm_codelabel_t *err;
	rvmcpu_t *cpu;
	rvm_opmap_t *opmap;
	unsigned int ntable;

	ctx.opmap = opmap = rvm_opmap_create();
	cpu = rvm_cpu_create_default();
	cpu->userdata1 = &ctx;
	cg = rvm_codegen_create();


	rvm_codemap_invalid_add_s(cg->codemap, "str_init");
	rvm_codemap_invalid_add_s(cg->codemap, "str_to_double");
	ntable = rvm_cpu_addswitable(cpu, "switable", switable);

	rvm_codegen_addins(cg, rvm_asmp(RVM_MOV, R0, DA, XX, hello));
	rvm_codegen_addins(cg, rvm_asm(RVM_STS, R0, SP, DA, 1 + RVM_CODEGEN_FUNCINITOFFSET));
	rvm_codegen_addins(cg, rvm_asml(RVM_MOV, R0, DA, XX, r_strlen(hello)));
	rvm_codegen_addins(cg, rvm_asm(RVM_STS, R0, SP, DA, 2 + RVM_CODEGEN_FUNCINITOFFSET));
	rvm_codegen_addrelocins_s(cg, RVM_RELOC_BRANCH, "str_init", rvm_asm(RVM_BL,  DA, XX, XX, 0));
	rvm_codegen_addins(cg, rvm_asmp(RVM_MOV, R7, R0, XX, 0));

	rvm_codegen_addins(cg, rvm_asmp(RVM_MOV, R0, DA, XX, there));
	rvm_codegen_addins(cg, rvm_asm(RVM_STS, R0, SP, DA, 1 + RVM_CODEGEN_FUNCINITOFFSET));
	rvm_codegen_addins(cg, rvm_asml(RVM_MOV, R0, DA, XX, r_strlen(there)));
	rvm_codegen_addins(cg, rvm_asm(RVM_STS, R0, SP, DA, 2 + RVM_CODEGEN_FUNCINITOFFSET));
	rvm_codegen_addrelocins_s(cg, RVM_RELOC_BRANCH, "str_init", rvm_asm(RVM_BL,  DA, XX, XX, 0));
	rvm_codegen_addins(cg, rvm_asmp(RVM_MOV, R8, R0, XX, 0));

	rvm_codegen_addins(cg, rvm_asmp(RVM_MOV, R0, R7, XX, 0));
	rvm_codegen_addins(cg, rvm_asm(RVM_OPSWI(rvm_cpu_swilookup_s(cpu, "switable", "cat")), R0, R0, R8, 0));
	rvm_codegen_addins(cg, rvm_asmp(RVM_MOV, R7, R0, XX, 0));

	rvm_codegen_addins(cg, rvm_asm(RVM_OPSWI(rvm_cpu_swilookup_s(cpu, "switable", "print")), R7, XX, XX, 0));	// print
	rvm_codegen_addins(cg, rvm_asm(RVM_OPSWI(rvm_cpu_swilookup_s(cpu, "switable", "print")), R8, XX, XX, 0));	// print
	rvm_codegen_addins(cg, rvm_asm(RVM_OPSWI(rvm_cpu_swilookup_s(cpu, "switable", "unref")), R7, XX, XX, 0));	// unref
	rvm_codegen_addins(cg, rvm_asm(RVM_OPSWI(rvm_cpu_swilookup_s(cpu, "switable", "unref")), R8, XX, XX, 0));	// unref
	rvm_codegen_addins(cg, rvm_asm(RVM_EXT, XX, XX, XX, 0));

	rvm_codegen_funcstart_s(cg, "str_init", 2);
	rvm_codegen_addins(cg, rvm_asm(RVM_LDS, R0, FP, DA, 1));
	rvm_codegen_addins(cg, rvm_asm(RVM_LDS, R1, FP, DA, 2));
	rvm_codegen_addins(cg, rvm_asm(RVM_OPSWI(rvm_cpu_swilookup_s(cpu, "switable", "str_init")), R0, R1, XX, 0));
	rvm_codegen_funcend(cg);

	rvm_codegen_funcstart_s(cg, "str_to_double", 1);
	rvm_codegen_addins(cg, rvm_asm(RVM_LDS, R0, FP, DA, 1));
	rvm_codegen_addins(cg, rvm_asm(RVM_OPSWI(rvm_cpu_swilookup_s(cpu, "switable", "str_to_double")), R0, R0, XX, 0));
	rvm_codegen_funcend(cg);

	if (rvm_codegen_relocate(cg, &err) < 0) {
		r_printf("Unresolved symbol: %s\n", err->name->str);
		goto end;
	}

	rvm_cpu_exec_debug(cpu, rvm_codegen_getcode(cg, 0), 0);

end:
	rvm_cpu_destroy(cpu);
	rvm_opmap_destroy(opmap);
	rvm_codegen_destroy(cg);

	fprintf(stdout, "Max alloc mem: %ld\n", r_debug_get_maxmem());
	fprintf(stdout, "Leaked mem: %ld\n", r_debug_get_allocmem());
	return 0;
}
