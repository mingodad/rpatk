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

#ifndef _REXNFASIMULATOR_H_
#define _REXNFASIMULATOR_H_

#include "rtypes.h"
#include "rlib/robject.h"
#include "rlib/rarray.h"
#include "rex/rexdb.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct rex_nfasimulator_s {
	rarray_t *accepts;
	rarray_t *newstates;
	rarray_t *oldstates;
	rarray_t *onmap;
	long inputsize;
	long count;
} rex_nfasimulator_t;


rex_nfasimulator_t *rex_nfasimulator_create();
void rex_nfasimulator_destroy(rex_nfasimulator_t *si);
long rex_nfasimulator_run(rex_nfasimulator_t *si, rexdb_t *nfa, long uid, const char *str, unsigned long size);
void rex_nfasimulator_start(rex_nfasimulator_t *si, rexdb_t *db, long uid);
long rex_nfasimulator_next(rex_nfasimulator_t *si, rexdb_t *db, ruint32 wc, int wcsize);


#ifdef __cplusplus
}
#endif

#endif
