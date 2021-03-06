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

#ifndef _RHASH_H_
#define _RHASH_H_

#include "rtypes.h"
#include "rlib/robject.h"
#include "rlib/rlist.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct rhash_node_s rhash_node_t;
typedef struct rhash_s rhash_t;
typedef rboolean (*r_hash_equalfunc)(rconstpointer key1, rconstpointer key2);
typedef unsigned int (*r_hash_hashfun)(rconstpointer key);


struct rhash_s {
	robject_t obj;
	rlist_t *buckets;
	unsigned int nbits;
	r_hash_equalfunc eqfunc;
	r_hash_hashfun hfunc;
};


#define R_HASH_INVALID_INDEXVAL ((unsigned long)-1)

#define r_hash_size(__h__) (((unsigned long)1) << (__h__)->nbits)
#define r_hash_mask(__h__) (r_hash_size(__h__) - 1)
rhash_t *r_hash_create(unsigned int nbits, r_hash_equalfunc eqfunc, r_hash_hashfun hfunc);
void r_hash_destroy(rhash_t* hash);
robject_t *r_hash_init(robject_t *obj, ruint32 type, r_object_cleanupfun cleanup, r_object_copyfun copy,
						unsigned int nbits, r_hash_equalfunc eqfunc, r_hash_hashfun hfunc);

void r_hash_insert(rhash_t* hash, rconstpointer key, rpointer value);
void r_hash_remove(rhash_t* hash, rconstpointer key);
void r_hash_removeall(rhash_t* hash);
rpointer r_hash_lookup(rhash_t* hash, rconstpointer key);
void r_hash_insert_indexval(rhash_t* hash, rconstpointer key, unsigned long index);
unsigned long r_hash_lookup_indexval(rhash_t* hash, rconstpointer key);
unsigned long r_hash_taillookup_indexval(rhash_t* hash, rconstpointer key);
rhash_node_t *r_hash_nodelookup(rhash_t* hash, rhash_node_t *cur, rconstpointer key);
rhash_node_t *r_hash_nodetaillookup(rhash_t* hash, rhash_node_t *cur, rconstpointer key);
rpointer r_hash_value(rhash_node_t *node);
unsigned long r_hash_indexval(rhash_node_t *node);

unsigned int r_hash_longhash(rconstpointer key);
rboolean r_hash_longequal(rconstpointer key1, rconstpointer key2);
unsigned int r_hash_strhash(rconstpointer key);
rboolean r_hash_strequal(rconstpointer key1, rconstpointer key2);
unsigned int r_hash_rstrhash(rconstpointer key);
rboolean r_hash_rstrequal(rconstpointer key1, rconstpointer key2);


/*
 * Virtual methods implementation
 */
void r_hash_cleanup(robject_t *obj);
robject_t *r_hash_copy(const robject_t *obj);


#ifdef __cplusplus
}
#endif

#endif
