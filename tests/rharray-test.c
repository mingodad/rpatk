#include <stdio.h>
#include <stdlib.h>
#include "rvmcodegen.h"
#include "rvmreg.h"

#include "rstring.h"
#include "rmem.h"
#include "rvmcpu.h"



static void test_swi_print_r(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	if (rvm_reg_gettype(RVM_CPUREG_PTR(cpu, ins->op2)) == RVM_DTYPE_LONG)
		fprintf(stdout, "R%d = %ld\n", ins->op2, RVM_CPUREG_GETL(cpu, ins->op2));
	else if (rvm_reg_gettype(RVM_CPUREG_PTR(cpu, ins->op2)) == RVM_DTYPE_DOUBLE)
		fprintf(stdout, "R%d = %5.2f\n", ins->op2, RVM_CPUREG_GETD(cpu, ins->op2));
	else if (rvm_reg_gettype(RVM_CPUREG_PTR(cpu, ins->op2)) == RVM_DTYPE_STRING)
		fprintf(stdout, "R%d = %s\n", ins->op2, ((rstring_t*) RVM_CPUREG_GETP(cpu, ins->op2))->s.str);
	else
		fprintf(stdout, "R%d = Unknown type\n", ins->op2);
}


static void test_swi_unref(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	if (rvm_reg_flagtst(RVM_CPUREG_PTR(cpu, ins->op2), RVM_INFOBIT_REFOBJECT))
		r_ref_dec((rref_t*)RVM_CPUREG_GETP(cpu, ins->op2));
}


static rvm_switable_t switable[] = {
		{"unref", test_swi_unref},
		{"print", test_swi_print_r},
		{NULL, NULL},
};


int main(int argc, char *argv[])
{
	rvmreg_t ag, rh, rt;
	rharray_t *na, *nc;
	rvm_codegen_t *cg;
	rhash_node_t *node;
	rvmcpu_t *cpu;

	cpu = rvm_cpu_create();
	rvmcpu_switable_add(cpu, switable);
	cg = rvm_codegen_create();

	ag = rvm_reg_create_double(4.55);
	rh = rvm_reg_create_string_ansi("Hello World");
	rt = rvm_reg_create_long(55);

	na = r_harray_create_rvmreg();
	r_harray_add_s(na, "again", &ag);
	r_harray_add_s(na, "again", &rh);
	r_harray_add_s(na, "hello", &rh);
	r_harray_add_s(na, "there", NULL);
	r_harray_set(na, r_harray_lookup_s(na, "there"), &rt);
	nc = (rharray_t*)r_ref_copy(&na->ref);

	fprintf(stdout, "lookup 'missing': %ld\n", r_harray_lookup_s(nc, "missing"));
	for (node = r_harray_nodelookup_s(nc, NULL, "again"); node; node = r_harray_nodelookup_s(nc, node, "again"))
		fprintf(stdout, "lookup 'again': %ld\n", r_hash_indexval(node));
	fprintf(stdout, "lookup 'hello': %ld\n", r_harray_lookup_s(nc, "hello"));
	fprintf(stdout, "lookup 'there': %ld\n", r_harray_lookup_s(nc, "there"));

	/*
	 * Load the content of rh to R0
	 */
	rvm_codegen_addins(cg, rvm_asmp(RVM_LDRR, R0, DA, XX, &rh));

	/*
	 * Lookup the array member "again" and load the content to R1
	 */
	rvm_codegen_addins(cg, rvm_asmp(RVM_LDRR, R1, DA, XX, r_harray_get(nc, r_harray_lookup_s(nc, "again"))));
	/*
	 * Lookup the array member "again" and load the content to R2
	 */
	rvm_codegen_addins(cg, rvm_asmp(RVM_LDRR, R2, DA, XX, r_harray_get(nc, r_harray_lookup_s(nc, "there"))));

	rvm_codegen_addins(cg, rvm_asm(RVM_SWI, DA, R0, XX, rvm_cpu_getswi(cpu, "print")));	// print
	rvm_codegen_addins(cg, rvm_asm(RVM_SWI, DA, R1, XX, rvm_cpu_getswi(cpu, "print")));	// print
	rvm_codegen_addins(cg, rvm_asm(RVM_SWI, DA, R2, XX, rvm_cpu_getswi(cpu, "print")));	// print
	rvm_codegen_addins(cg, rvm_asm(RVM_EXT, XX, XX, XX, 0));

	rvm_relocate(rvm_codegen_getcode(cg, 0), rvm_codegen_getcodesize(cg));
	rvm_cpu_exec_debug(cpu, rvm_codegen_getcode(cg, 0), 0);
	rvm_cpu_destroy(cpu);
	rvm_codegen_destroy(cg);


	r_ref_dec((rref_t*)na);
	r_ref_dec((rref_t*)nc);
	fprintf(stdout, "Max alloc mem: %ld\n", r_debug_get_maxmem());
	fprintf(stdout, "Leaked mem: %ld\n", r_debug_get_allocmem());
	return 0;
}
