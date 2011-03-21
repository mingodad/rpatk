#ifndef _RPAVM_H_
#define _RPAVM_H_

#include "rvmcpu.h"
#include "rparecord.h"


#ifdef __cplusplus
extern "C" {
#endif


#define RPA_MATCH_NONE 0
#define RPA_MATCH_MULTIPLE (1 << 0)
#define RPA_MATCH_OPTIONAL (1 << 1)
#define RPA_MATCH_MULTIOPT (RPA_MATCH_MULTIPLE | RPA_MATCH_OPTIONAL)

#define R_ARG R7
#define R_TOP R8
#define R_LOO TP
#define R_WHT FP
#define RPAVM_SWI_TABLEID 0

#define RPA_MATCHCHR_NAN	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 0))
#define RPA_MATCHCHR_OPT	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 1))
#define RPA_MATCHCHR_MUL	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 2))
#define RPA_MATCHCHR_MOP	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 3))

#define RPA_MATCHRNG_NAN	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 4))
#define RPA_MATCHRNG_OPT	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 5))
#define RPA_MATCHRNG_MUL	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 6))
#define RPA_MATCHRNG_MOP	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 7))

#define RPA_MATCHSPCHR_NAN	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 8))
#define RPA_MATCHSPCHR_OPT	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 9))
#define RPA_MATCHSPCHR_MUL	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 10))
#define RPA_MATCHSPCHR_MOP	RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 11))

#define RPA_SHIFT			RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 12))
#define RPA_EMITSTART		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 13))
#define RPA_EMITEND			RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 14))
#define RPA_BXLWHT			RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 15))
#define RPA_GETRECLEN		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 16))
#define RPA_SETRECLEN		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 17))
#define RPA_SETRECID		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 18))
#define RPA_LOOPDETECT		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 19))
#define RPA_SETCACHE		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 20))
#define RPA_CHECKCACHE		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 21))
#define RPA_BXLNAN			RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 22))
#define RPA_BXLOPT			RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 23))
#define RPA_BXLMUL			RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 24))
#define RPA_BXLMOP			RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 25))
#define RPA_SETBXLNAN		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 26))
#define RPA_SETBXLOPT		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 27))
#define RPA_SETBXLMUL		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 28))
#define RPA_SETBXLMOP		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 29))
#define RPA_SETRECUID		RVM_OPSWI(RVM_SWI_ID(RPAVM_SWI_TABLEID, 30))



typedef struct rpainput_s {
	const rchar *input;
	ruint32 wc;
	ruchar eof;
} rpainput_t;


typedef struct rpainmap_s {
	const rchar *input;
	rulong serial;
} rpainmap_t;


rvmcpu_t *rpavm_cpu_create(rulong stacksize);
void rpavm_cpu_destroy(rvmcpu_t * vm);


#ifdef __cplusplus
}
#endif

#endif
