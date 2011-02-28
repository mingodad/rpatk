#include "rmem.h"
#include "rpacompiler.h"
#include "rstring.h"


void rpacompiler_mnode_nan(rpa_compiler_t *co)
{
	rvm_codegen_addlabel_s(co->cg, "rpacompiler_mnode_nan");
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, R_WHT, XX, XX, 0));
}


void rpacompiler_mnode_opt(rpa_compiler_t *co)
{
	rvm_codegen_addlabel_s(co->cg, "rpacompiler_mnode_opt");
	rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSH, LR, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BXL, R_WHT, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POP, LR, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_CMP, R0, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BXGRE, LR, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_CMP, R0, R0, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
}


void rpacompiler_mnode_mul(rpa_compiler_t *co)
{
	rvm_codegen_addlabel_s(co->cg, "rpacompiler_mnode_mul");
	rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSH, LR, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BXL, R_WHT, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_CMP, R0, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BGRE, DA, XX, XX, 2));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POP, PC, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_CLR, R1, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_ADD, R0, R0, R1, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSH, R0, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BXL, R_WHT, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POP, R1, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_CMP, R0, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BGRE, DA, XX, XX, -5));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_ADDS, R0, R1, DA, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POP, PC, XX, XX, 0));
}


void rpacompiler_mnode_mop(rpa_compiler_t *co)
{
	rvm_codegen_addlabel_s(co->cg, "rpacompiler_mnode_mop");
	rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSH, LR, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BXL, R_WHT, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_CMP, R0, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BGRE, DA, XX, XX, 4));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_CMP, R0, R0, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POP, PC, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_CLR, R1, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_ADD, R0, R0, R1, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSH, R0, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BXL, R_WHT, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POP, R1, XX, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asml(RVM_CMP, R0, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BGRE, DA, XX, XX, -5));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_ADDS, R0, R1, DA, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POP, PC, XX, XX, 0));
}


rpa_compiler_t *rpa_compiler_create()
{
	rpa_compiler_t *co;

	co = r_malloc(sizeof(*co));
	r_memset(co, 0, sizeof(*co));
	co->cg = rvm_codegen_create();
	co->scope = rvm_scope_create();
	co->expressions = r_array_create(sizeof(rpa_ruledef_t));
	rpacompiler_mnode_nan(co);
	rpacompiler_mnode_opt(co);
	rpacompiler_mnode_mul(co);
	rpacompiler_mnode_mop(co);
	return co;
}


void rpa_compiler_destroy(rpa_compiler_t *co)
{
	if (co) {
		rvm_codegen_destroy(co->cg);
		rvm_scope_destroy(co->scope);
		r_object_destroy((robject_t*)co->expressions);
	}
	r_free(co);
}


rint rpa_compiler_rule_begin(rpa_compiler_t *co, const rchar *name, ruint namesize)
{
	rpa_ruledef_t exp;
	rchar endlabel[64];

	r_memset(&exp, 0, sizeof(exp));
	exp.emitidx = rvm_codegen_adddata_s(co->cg, NULL, name, namesize);
	exp.labelidx = rvm_codegen_addlabel(co->cg, name, namesize);
	exp.start = rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_STRING, exp.emitidx, rvm_asm(RPA_EMITSTART, DA, R_TOP, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSHM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__end:%ld", exp.start);
	exp.endidx = rvm_codemap_invalid_add_s(co->cg->codemap, endlabel);
	r_array_add(co->expressions, &exp);
	return 0;
}


rint rpa_compiler_rule_begin_s(rpa_compiler_t *co, const rchar *name)
{
	return rpa_compiler_rule_begin(co, name, r_strlen(name));
}


rint rpa_compiler_rule_end(rpa_compiler_t *co)
{
	rpa_ruledef_t exp = r_array_pop(co->expressions, rpa_ruledef_t);

	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R1)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_SUBS, R0, R_TOP, R1, 0));
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_STRING, exp.emitidx, rvm_asm(RPA_EMITEND, DA, R1, R0, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_redefinelabel(co->cg, exp.endidx);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	return 0;
}


rint rpa_compiler_exp_begin(rpa_compiler_t *co)
{
	rpa_ruledef_t exp;
	rchar endlabel[64];

	exp.branch = rvm_codegen_addins(co->cg, rvm_asm(RVM_B, DA, XX, XX, 0));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__begin:%ld", rvm_codegen_getcodesize(co->cg));
	exp.labelidx = rvm_codegen_addlabel_s(co->cg, endlabel);
	exp.start = rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSHM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__end:%ld", exp.start);
	exp.endidx = rvm_codemap_invalid_add_s(co->cg->codemap, endlabel);
	r_array_add(co->expressions, &exp);
	return 0;
}


rint rpa_compiler_exp_end(rpa_compiler_t *co, ruint qflag)
{
	rpa_ruledef_t exp = r_array_pop(co->expressions, rpa_ruledef_t);

	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R1)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_SUBS, R0, R_TOP, R1, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_redefinelabel(co->cg, exp.endidx);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_replaceins(co->cg, exp.branch, rvm_asm(RVM_B, DA, XX, XX, rvm_codegen_getcodesize(co->cg) - exp.branch));
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_JUMP, exp.labelidx, rvm_asm(RVM_MOV, R_WHT, DA, XX, 0));
	if (qflag == RPA_MATCH_OPTIONAL) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_opt", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIPLE) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mul", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIOPT) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mop", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_nan", rvm_asm(RVM_BL, DA, XX, XX, 0));
	}
	return 0;
}


rint rpa_compiler_altexp_begin(rpa_compiler_t *co)
{
	rpa_ruledef_t exp;
	rchar endlabel[64];

	exp.branch = rvm_codegen_addins(co->cg, rvm_asm(RVM_B, DA, XX, XX, 0));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__begin:%ld", rvm_codegen_getcodesize(co->cg));
	exp.labelidx = rvm_codegen_addlabel_s(co->cg, endlabel);
	exp.start = rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSHM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__end:%ld", exp.start);
	exp.endidx = rvm_codemap_invalid_add_s(co->cg->codemap, endlabel);
	r_array_add(co->expressions, &exp);
	return 0;
}


rint rpa_compiler_altexp_end(rpa_compiler_t *co, ruint qflag)
{
	rpa_ruledef_t exp = r_array_pop(co->expressions, rpa_ruledef_t);

	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_redefinelabel(co->cg, exp.endidx);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R1)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_SUBS, R0, R_TOP, R1, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));

	rvm_codegen_replaceins(co->cg, exp.branch, rvm_asm(RVM_B, DA, XX, XX, rvm_codegen_getcodesize(co->cg) - exp.branch));
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_JUMP, exp.labelidx, rvm_asm(RVM_MOV, R_WHT, DA, XX, 0));
	if (qflag == RPA_MATCH_OPTIONAL) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_opt", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIPLE) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mul", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIOPT) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mop", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_nan", rvm_asm(RVM_BL, DA, XX, XX, 0));
	}
	return 0;
}


rint rpa_compiler_branch_begin(rpa_compiler_t *co)
{
	rpa_ruledef_t exp;
	rchar endlabel[64];

	exp.branch = rvm_codegen_addins(co->cg, rvm_asm(RVM_B, DA, XX, XX, 0));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__begin:%ld", rvm_codegen_getcodesize(co->cg));
	exp.labelidx = rvm_codegen_addlabel_s(co->cg, endlabel);
	exp.start = rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSHM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__end:%ld", exp.start);
	exp.endidx = rvm_codemap_invalid_add_s(co->cg->codemap, endlabel);
	r_array_add(co->expressions, &exp);
	return 0;
}


rint rpa_compiler_branch_end(rpa_compiler_t *co, ruint qflag)
{
	rpa_ruledef_t exp = r_array_pop(co->expressions, rpa_ruledef_t);

	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R1)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_SUBS, R0, R_TOP, R1, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_redefinelabel(co->cg, exp.endidx);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_replaceins(co->cg, exp.branch, rvm_asm(RVM_B, DA, XX, XX, rvm_codegen_getcodesize(co->cg) - exp.branch));
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_JUMP, exp.labelidx, rvm_asm(RVM_MOV, R_WHT, DA, XX, 0));
	if (qflag == RPA_MATCH_OPTIONAL) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_opt", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIPLE) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mul", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIOPT) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mop", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_nan", rvm_asm(RVM_BL, DA, XX, XX, 0));
	}
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_BRANCH, RPA_COMPILER_CURRENTEXP(co)->endidx, rvm_asm(RVM_BGRE, DA, XX, XX, 0));

	return 0;
}


rint rpa_compiler_class_begin(rpa_compiler_t *co)
{
	rpa_ruledef_t exp;
	rchar endlabel[64];

	exp.branch = rvm_codegen_addins(co->cg, rvm_asm(RVM_B, DA, XX, XX, 0));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__begin:%ld", rvm_codegen_getcodesize(co->cg));
	exp.labelidx = rvm_codegen_addlabel_s(co->cg, endlabel);
	exp.start = rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSHM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__end:%ld", exp.start);
	exp.endidx = rvm_codemap_invalid_add_s(co->cg->codemap, endlabel);
	r_array_add(co->expressions, &exp);
	return 0;
}


rint rpa_compiler_class_end(rpa_compiler_t *co, ruint qflag)
{
	rpa_ruledef_t exp = r_array_pop(co->expressions, rpa_ruledef_t);

	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_redefinelabel(co->cg, exp.endidx);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R1)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_SUBS, R0, R_TOP, R1, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));

	rvm_codegen_replaceins(co->cg, exp.branch, rvm_asm(RVM_B, DA, XX, XX, rvm_codegen_getcodesize(co->cg) - exp.branch));
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_JUMP, exp.labelidx, rvm_asm(RVM_MOV, R_WHT, DA, XX, 0));
	if (qflag == RPA_MATCH_OPTIONAL) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_opt", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIPLE) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mul", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIOPT) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mop", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_nan", rvm_asm(RVM_BL, DA, XX, XX, 0));
	}
	return 0;
}


rint rpa_compiler_notexp_begin(rpa_compiler_t *co)
{
	rpa_ruledef_t exp;
	rchar endlabel[64];

	exp.branch = rvm_codegen_addins(co->cg, rvm_asm(RVM_B, DA, XX, XX, 0));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__begin:%ld", rvm_codegen_getcodesize(co->cg));
	exp.labelidx = rvm_codegen_addlabel_s(co->cg, endlabel);
	exp.start = rvm_codegen_addins(co->cg, rvm_asm(RVM_PUSHM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	r_snprintf(endlabel, sizeof(endlabel) - 1, "__end:%ld", exp.start);
	exp.endidx = rvm_codemap_invalid_add_s(co->cg->codemap, endlabel);
	r_array_add(co->expressions, &exp);
	return 0;
}


rint rpa_compiler_notexp_end(rpa_compiler_t *co, ruint qflag)
{
	rpa_ruledef_t exp = r_array_pop(co->expressions, rpa_ruledef_t);

	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_MOV, R0, DA, XX, -1));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_CMP, R0, DA, XX, 0));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));
	rvm_codegen_redefinelabel(co->cg, exp.endidx);
	rvm_codegen_addins(co->cg, rvm_asm(RVM_POPM, DA, XX, XX, BIT(R_TOP)|BIT(R_WHT)|BIT(LR)));
	rvm_codegen_addins(co->cg, rvm_asm(RPA_MATCHSPCHR_NAN, DA, XX, XX, '.'));
	rvm_codegen_addins(co->cg, rvm_asm(RVM_BX, LR, XX, XX, 0));

	rvm_codegen_replaceins(co->cg, exp.branch, rvm_asm(RVM_B, DA, XX, XX, rvm_codegen_getcodesize(co->cg) - exp.branch));
	rvm_codegen_index_addrelocins(co->cg, RVM_RELOC_JUMP, exp.labelidx, rvm_asm(RVM_MOV, R_WHT, DA, XX, 0));
	if (qflag == RPA_MATCH_OPTIONAL) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_opt", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIPLE) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mul", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else if (qflag == RPA_MATCH_MULTIOPT) {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_mop", rvm_asm(RVM_BL, DA, XX, XX, 0));
	} else {
		rvm_codegen_addrelocins_s(co->cg, RVM_RELOC_BRANCH, "rpacompiler_mnode_nan", rvm_asm(RVM_BL, DA, XX, XX, 0));
	}
	return 0;
}
