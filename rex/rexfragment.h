/*
 *  Regular Pattern Analyzer (RPA)
 *  Copyright (c) 2009-2010 Martin Stoilov
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

#ifndef _REXFRAGMENT_H_
#define _REXFRAGMENT_H_

#include "rtypes.h"
#include "rex/rexstate.h"
#include "rex/rexdb.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct rexfragment_s {
	rexstate_t *sstate;
	rarray_t *dangling;
} rexfragment_t;

#define REX_FRAG_STATEUID(__f__) (__f__)->sstate->uid
#define REX_FRAG_STATE(__f__) (__f__)->sstate

void rex_fragment_destroy(rexfragment_t *frag);
rexfragment_t *rex_fragment_create(rexstate_t *state);
rexfragment_t *rex_fragment_cat(rexdb_t *rexdb, rexfragment_t *frag1, rexfragment_t *frag2);
rexfragment_t *rex_fragment_alt(rexdb_t *rexdb, rexfragment_t *frag1, rexfragment_t *frag2);
rexfragment_t *rex_fragment_opt(rexdb_t *rexdb, rexfragment_t *frag1);
rexfragment_t *rex_fragment_mop(rexdb_t *rexdb, rexfragment_t *frag1);
rexfragment_t *rex_fragment_mul(rexdb_t *rexdb, rexfragment_t *frag1);


#ifdef __cplusplus
}
#endif


#endif /* REXFRAGMENT_H_ */