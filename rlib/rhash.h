#ifndef _RHASH_H_
#define _RHASH_H_

#include "rtypes.h"
#include "rref.h"
#include "rlist.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct rhash_node_s rhash_node_t;
typedef struct rhash_s rhash_t;
typedef rboolean (*r_hash_equalfunc)(rconstpointer key1, rconstpointer key2);
typedef ruint (*r_hash_hashfun)(rconstpointer key);


struct rhash_s {
	rref_t ref;
	rlist_t *buckets;
	ruint nbits;
	r_hash_equalfunc eqfunc;
	r_hash_hashfun hfunc;
};


#define R_HASH_INVALID_INDEXVAL ((ruint)-1)

#define r_hash_size(__h__) (1 << (__h__)->nbits)
#define r_hash_mask(__h__) (r_hash_size(__h__) - 1)
rhash_t *r_hash_create(ruint nbits, r_hash_equalfunc eqfunc, r_hash_hashfun hfunc);
rhash_t *r_hash_init(rhash_t *hash, ruint nbits, r_hash_equalfunc eqfunc, r_hash_hashfun hfunc);
rhash_t *r_hash_copy(const rhash_t *hash);
void r_hash_destroy(rhash_t *hash);
void r_hash_cleanup(rhash_t *hash);
void r_hash_insert(rhash_t* hash, rconstpointer key, rpointer value);
void r_hash_remove(rhash_t* hash, rconstpointer key);
void r_hash_removeall(rhash_t* hash);
rpointer r_hash_lookup(rhash_t* hash, rconstpointer key);
void r_hash_insert_indexval(rhash_t* hash, rconstpointer key, rulong index);
rulong r_hash_lookup_indexval(rhash_t* hash, rconstpointer key);
rhash_node_t *r_hash_node_lookup(rhash_t* hash, rhash_node_t *cur, rconstpointer key);
rpointer r_hash_value(rhash_node_t *node);
rulong r_hash_indexval(rhash_node_t *node);

ruint r_hash_strhash(rconstpointer key);
rboolean r_hash_strequal(rconstpointer key1, rconstpointer key2);
ruint r_hash_strnhash(rconstpointer key);
rboolean r_hash_strnequal(rconstpointer key1, rconstpointer key2);


#ifdef __cplusplus
}
#endif

#endif
