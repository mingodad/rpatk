#ifndef __RVMOPERATORLSRU_H_
#define __RVMOPERATORLSRU_H_

#include "rvmoperator.h"
#include "rvmreg.h"

#ifdef __cplusplus
extern "C" {
#endif

void rvm_op_lsru_unsigned(rvmcpu_t *cpu, rushort opid, rvmreg_t *res, rword op1, rword op2);
void rvm_op_lsru_long(rvmcpu_t *cpu, rushort opid, rvmreg_t *res, rlong op1, rlong op2);
void rvm_op_lsru_double(rvmcpu_t *cpu, rushort opid, rvmreg_t *res, rdouble op1, rdouble op2);

#ifdef __cplusplus
}
#endif

#endif