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


#ifndef _REXCC_H_
#define _REXCC_H_

#include <stdio.h>
#include "rlib/rbuffer.h"
#include "rlib/rarray.h"
#include "rex/rexdb.h"
#include "rex/rexdfa.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct rexcc_s {
	rexdb_t *nfa;
	rexdfa_t *dfa;
	rarray_t *parseinfo;
	long startuid;
	unsigned int showtime;
	rbuffer_t *buffer;
	rbuffer_t prolog;
	rbuffer_t epilog;
	char *temp;
	int token;
	char *input;
	char *start;
	char *end;
	char *tokenptr;
	int tokenlen;
} rexcc_t;


rexcc_t *rex_cc_create();
void rex_cc_destroy(rexcc_t *pCC);
int rex_cc_load_pattern(rexcc_t *pCC, rbuffer_t *buf, rexuserdata_t userdata);
int rex_cc_load_buffer(rexcc_t *pCC, rbuffer_t *text);
int rex_cc_output(rexcc_t *pCC, FILE *outc);
rexdfa_t * rex_cc_tokensdfa();
void rex_cc_parseinfodump(rexcc_t *pCC);


#ifdef __cplusplus
}
#endif

#endif
