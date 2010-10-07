#include <stdio.h>
#include <stdlib.h>
#include "rstring.h"
#include "rhash.h"
#include "rmem.h"
#include "rvmcodemap.h"


void codelabel_print_info(rvm_codemap_t *codemap, rchar* name)
{
	rvm_codelabel_t *label;

	label = rvm_codemap_strlookup(codemap, name);
	if (!label)
		fprintf(stdout, "%s (not found)\n", name);
	else
		fprintf(stdout, "%s, asmins: 0x%d\n", label->name->str, (ruint)label->index);
}


int main(int argc, char *argv[])
{
	rvm_codemap_t *codemap = rvm_codemap_create();

	rvm_codemap_stradd(codemap, "add2", 0);
	rvm_codemap_stradd(codemap, "add3", 3);
	rvm_codemap_stradd(codemap, "sub2", 7);

	codelabel_print_info(codemap, "add2");
	codelabel_print_info(codemap, "add7");
	codelabel_print_info(codemap, "sub2");

	rvm_codemap_destroy(codemap);

	fprintf(stdout, "Max alloc mem: %ld\n", r_debug_get_maxmem());
	fprintf(stdout, "Leaked mem: %ld\n", r_debug_get_allocmem());
	return 0;
}
