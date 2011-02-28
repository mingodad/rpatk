#ifndef _RPACOMPILER_H_
#define _RPACOMPILER_H_

#include "rvmcodegen.h"
#include "rvmscope.h"
#include "rpavm.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rpa_ruledef_s {
	rulong branch;
	rulong start;
	rlong labelidx;
	rlong emitidx;
	rlong endidx;
} rpa_ruledef_t;


#define RPA_COMPILER_CURRENTEXP(__co__) ((rpa_ruledef_t*)r_array_lastslot((__co__)->expressions))

typedef struct rpa_compiler_s {
	rvm_codegen_t *cg;
	rarray_t *expressions;
	rboolean optimized;
	rvm_scope_t *scope;
	rulong fpoff;
} rpa_compiler_t;


rpa_compiler_t *rpa_compiler_create();
void rpa_compiler_destroy(rpa_compiler_t *co);
rint rpa_compiler_rule_begin(rpa_compiler_t *co, const rchar *name, ruint namesize);
rint rpa_compiler_rule_begin_s(rpa_compiler_t *co, const rchar *name);
rint rpa_compiler_rule_end(rpa_compiler_t *co);
rint rpa_compiler_exp_begin(rpa_compiler_t *co);
rint rpa_compiler_exp_end(rpa_compiler_t *co, ruint qflag);

rint rpa_compiler_altbranch_begin(rpa_compiler_t *co);
rint rpa_compiler_altbranch_end(rpa_compiler_t *co, ruint qflag);
rint rpa_compiler_altexp_begin(rpa_compiler_t *co);
rint rpa_compiler_altexp_end(rpa_compiler_t *co, ruint qflag);


#ifdef __cplusplus
}
#endif

#endif
