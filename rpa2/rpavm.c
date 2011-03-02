#include "rpavm.h"
#include "rpastat.h"
#include "rutf.h"


static void rpavm_swi_shift(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpastat_t *stat = (rpastat_t *)cpu->userdata1;
	rlong tp = RVM_CPUREG_GETL(cpu, R_TOP);
	rpainput_t * ptp = &stat->instack[tp];

	if (ptp->eof)
		return;
	ptp++;
	tp++;
	if (tp >= (rlong)stat->ip.serial) {
		rint inc = 0;
		ptp->input = stat->ip.input;
		if (ptp->input < stat->end) {
			inc = r_utf8_mbtowc(&ptp->wc, (const ruchar*)stat->ip.input, (const ruchar*)stat->end);
			stat->ip.input += inc;
			stat->ip.serial += 1;
			ptp->eof = 0;
		} else {
			ptp->wc = (ruint32)-1;
			ptp->eof = 1;
		}
	}
	RVM_CPUREG_SETL(cpu, R_TOP, tp);
}


static void rpavm_matchchr_do(rvmcpu_t *cpu, rvm_asmins_t *ins, rword flags)
{
	rpastat_t *stat = (rpastat_t *)cpu->userdata1;
	rword wc = RVM_CPUREG_GETU(cpu, ins->op1);
	rword matched = 0;

	if (flags == RPA_MATCH_OPTIONAL) {
		if (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof && stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc == wc) {
			rpavm_swi_shift(cpu, ins);
			matched = 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_Z;
		RVM_CPUREG_SETU(cpu, R0, matched);
	} else if (flags == RPA_MATCH_MULTIPLE) {
		while (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof && stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc == wc) {
			rpavm_swi_shift(cpu, ins);
			matched += 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_N;
		RVM_CPUREG_SETU(cpu, R0, matched ? matched : (rword)-1);
	} else if (flags == RPA_MATCH_MULTIOPT) {
		while (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof && stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc == wc) {
			rpavm_swi_shift(cpu, ins);
			matched += 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_Z;
		RVM_CPUREG_SETU(cpu, R0, matched );
	} else {
		if (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof && stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc == wc) {
			rpavm_swi_shift(cpu, ins);
			matched = 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_N;
		RVM_CPUREG_SETU(cpu, R0, matched ? matched : (rword)-1);
	}
}


static void rpavm_matchspchr_do(rvmcpu_t *cpu, rvm_asmins_t *ins, rword flags)
{
	rpastat_t *stat = (rpastat_t *)cpu->userdata1;
	rword wc = RVM_CPUREG_GETU(cpu, ins->op1);
	rword matched = 0;

	switch (wc) {
		case 't':
			wc = '\t';
			break;
		case 'r':
			wc = '\r';
			break;
		case 'n':
			wc = '\n';
			break;
		case '.':
			wc = (rword)-1;
			break;
		default:
			break;
	};

	if (flags == RPA_MATCH_OPTIONAL) {
		if (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof && (wc == (rword)-1 || stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc == wc)) {
			rpavm_swi_shift(cpu, ins);
			matched = 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_Z;
		RVM_CPUREG_SETU(cpu, R0, matched);
	} else if (flags == RPA_MATCH_MULTIPLE) {
		while (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof && (wc == (rword)-1 || stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc == wc)) {
			rpavm_swi_shift(cpu, ins);
			matched += 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_N;
		RVM_CPUREG_SETU(cpu, R0, matched ? matched : (rword)-1);
	} else if (flags == RPA_MATCH_MULTIOPT) {
		while (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof && (wc == (rword)-1 || stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc == wc)) {
			rpavm_swi_shift(cpu, ins);
			matched += 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_Z;
		RVM_CPUREG_SETU(cpu, R0, matched );
	} else {
		if (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof && (wc == (rword)-1 || stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc == wc)) {
			rpavm_swi_shift(cpu, ins);
			matched = 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_N;
		RVM_CPUREG_SETU(cpu, R0, matched ? matched : (rword)-1);
	}
}


static void rpavm_swi_matchchr_nan(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchchr_do(cpu, ins, RPA_MATCH_NONE);
}


static void rpavm_swi_matchchr_opt(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchchr_do(cpu, ins, RPA_MATCH_OPTIONAL);
}


static void rpavm_swi_matchchr_mul(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchchr_do(cpu, ins, RPA_MATCH_MULTIPLE);
}


static void rpavm_swi_matchchr_mop(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchchr_do(cpu, ins, RPA_MATCH_MULTIOPT);
}




static void rpavm_swi_matchspchr_nan(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchspchr_do(cpu, ins, RPA_MATCH_NONE);
}


static void rpavm_swi_matchspchr_opt(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchspchr_do(cpu, ins, RPA_MATCH_OPTIONAL);
}


static void rpavm_swi_matchspchr_mul(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchspchr_do(cpu, ins, RPA_MATCH_MULTIPLE);
}


static void rpavm_swi_matchspchr_mop(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchspchr_do(cpu, ins, RPA_MATCH_MULTIOPT);
}


static void rpavm_matchrng_do(rvmcpu_t *cpu, rvm_asmins_t *ins, rword flags)
{
	rpastat_t *stat = (rpastat_t *)cpu->userdata1;
	rpair_t pr = RVM_CPUREG_GETPAIR(cpu, ins->op1);
	rword matched = 0;

	if (flags == RPA_MATCH_OPTIONAL) {
		if (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof &&
			(stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc >= pr.p1 && stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc <= pr.p2)) {
			rpavm_swi_shift(cpu, ins);
			matched = 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_Z;
		RVM_CPUREG_SETU(cpu, R0, matched);
	} else if (flags == RPA_MATCH_MULTIPLE) {
		while (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof &&
				(stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc >= pr.p1 && stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc <= pr.p2)) {
			rpavm_swi_shift(cpu, ins);
			matched += 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_N;
		RVM_CPUREG_SETU(cpu, R0, matched ? matched : (rword)-1);
	} else if (flags == RPA_MATCH_MULTIOPT) {
		while (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof &&
				(stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc >= pr.p1 && stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc <= pr.p2)) {
			rpavm_swi_shift(cpu, ins);
			matched += 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_Z;
		RVM_CPUREG_SETU(cpu, R0, matched );
	} else {
		if (!stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].eof &&
			(stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc >= pr.p1 && stat->instack[RVM_CPUREG_GETL(cpu, R_TOP)].wc <= pr.p2)) {
			rpavm_swi_shift(cpu, ins);
			matched = 1;
		}
		cpu->status = matched ? 0 : RVM_STATUS_N;
		RVM_CPUREG_SETU(cpu, R0, matched ? matched : (rword)-1);
	}
}


static void rpavm_swi_matchrng_nan(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchrng_do(cpu, ins, RPA_MATCH_NONE);
}


static void rpavm_swi_matchrng_opt(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchrng_do(cpu, ins, RPA_MATCH_OPTIONAL);
}


static void rpavm_swi_matchrng_mul(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchrng_do(cpu, ins, RPA_MATCH_MULTIPLE);
}


static void rpavm_swi_matchrng_mop(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpavm_matchrng_do(cpu, ins, RPA_MATCH_MULTIOPT);
}



static void rpavm_swi_emitstart(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpastat_t *stat = (rpastat_t *)cpu->userdata1;
	rparecord_t *rec;
	rlong index;
	rword tp = RVM_CPUREG_GETU(cpu, ins->op2);
	rstr_t name = {RVM_CPUREG_GETSTR(cpu, ins->op1), RVM_CPUREG_GETSIZE(cpu, ins->op1)};

	index = r_array_add(stat->records, NULL);
	rec = (rparecord_t *)r_array_slot(stat->records, index);
	rec->rule = name.str;
	rec->top = tp;
	rec->type = RPA_RECORD_START;
//	r_printf("START: %s(%ld)\n", name.str, (rulong)tp);
}


static void rpavm_swi_emitend(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpastat_t *stat = (rpastat_t *)cpu->userdata1;
	rparecord_t *rec;
	rlong index;
	rword tp = RVM_CPUREG_GETU(cpu, ins->op2);
	rword tplen = RVM_CPUREG_GETU(cpu, ins->op3);
	rstr_t name = {RVM_CPUREG_GETSTR(cpu, ins->op1), RVM_CPUREG_GETSIZE(cpu, ins->op1)};

	index = r_array_add(stat->records, NULL);
	rec = (rparecord_t *)r_array_slot(stat->records, index);
	rec->rule = name.str;
	rec->top = tp;
	rec->size = tplen;
	rec->type = RPA_RECORD_START;

	if (tplen) {
		rec->type = RPA_RECORD_END | RPA_RECORD_MATCH;
//		r_printf("MATCHED: %s(%ld, %ld): %p(%d)\n", name.str, (rulong)tp, (rulong)tplen, name.str, name.size);
	} else {
		rec->type = RPA_RECORD_END;
//		r_printf("MATCHED: %s(%ld, %ld)\n", name.str, (rulong)tp, (rulong)tplen);
	}
}


static void rpavm_swi_bxlwht(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rword wht = RVM_CPUREG_GETU(cpu, ins->op2);

	RVM_CPUREG_SETU(cpu, R_WHT, wht);
	RVM_CPUREG_SETIP(cpu, LR, RVM_CPUREG_GETIP(cpu, PC));
	RVM_CPUREG_SETIP(cpu, PC, RVM_CPUREG_GETIP(cpu, ins->op1));
}


static void rpavm_swi_getreclen(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpastat_t *stat = (rpastat_t *)cpu->userdata1;
	RVM_CPUREG_SETU(cpu, ins->op1, r_array_length(stat->records));
}


static void rpavm_swi_setreclen(rvmcpu_t *cpu, rvm_asmins_t *ins)
{
	rpastat_t *stat = (rpastat_t *)cpu->userdata1;

	r_array_setlength(stat->records, (ruint)RVM_CPUREG_GETU(cpu, ins->op1));
}


static rvm_switable_t rpavm_swi_table[] = {
		{"RPA_MATCHCHR_NAN", rpavm_swi_matchchr_nan},
		{"RPA_MATCHCHR_OPT", rpavm_swi_matchchr_opt},
		{"RPA_MATCHCHR_MUL", rpavm_swi_matchchr_mul},
		{"RPA_MATCHCHR_MOP", rpavm_swi_matchchr_mop},
		{"RPA_MATCHRNG_NAN", rpavm_swi_matchrng_nan},
		{"RPA_MATCHRNG_OPT", rpavm_swi_matchrng_opt},
		{"RPA_MATCHRNG_MUL", rpavm_swi_matchrng_mul},
		{"RPA_MATCHRNG_MOP", rpavm_swi_matchrng_mop},
		{"RPA_MATCHSPCHR_NAN", rpavm_swi_matchspchr_nan},
		{"RPA_MATCHSPCHR_OPT", rpavm_swi_matchspchr_opt},
		{"RPA_MATCHSPCHR_MUL", rpavm_swi_matchspchr_mul},
		{"RPA_MATCHSPCHR_MOP", rpavm_swi_matchspchr_mop},
		{"RPA_SHIFT", rpavm_swi_shift},
		{"RPA_EMITSTART", rpavm_swi_emitstart},
		{"RPA_EMITEND", rpavm_swi_emitend},
		{"RPA_BXLWHT", rpavm_swi_bxlwht},
		{"RPA_GETRECLEN", rpavm_swi_getreclen},
		{"RPA_SETRECLEN", rpavm_swi_setreclen},
		{NULL, NULL},
};


rvmcpu_t *rpavm_cpu_create(rulong stacksize)
{
	rvmcpu_t *cpu = rvm_cpu_create(stacksize);
	rint tableid = rvm_cpu_addswitable(cpu, rpavm_swi_table);

	if (tableid != RPAVM_SWI_TABLEID) {
		rpavm_cpu_destroy(cpu);
		return NULL;
	}
	return cpu;
}


void rpavm_cpu_destroy(rvmcpu_t * cpu)
{
	rvm_cpu_destroy(cpu);
}

