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

#include "rmem.h"
#include "rstring.h"
#include "rpamatch.h"
#include "rpalist.h"
#include "rpamatchlist.h"
#include "rpamnode.h"
#include "rpastat.h"
#include "rpadbex.h"

#define RPA_MAX_RECURSION 100
#define RPA_DLOOP_INIT(__name__, __match__, __input__) {{&(__name__).lnk, &(__name__).lnk }, (__match__), (__input__), 0, 0}


static unsigned int rpa_mnode_getid(rpa_class_t *cls)
{
	return (RPA_MNODE_CLASSID);
}


static const char *rpa_mnode_getstr(rpa_class_t *cls)
{
	if ((((rpa_mnode_t*)cls)->flags & (RPA_MATCH_OPTIONAL|RPA_MATCH_MULTIPLE)) == (RPA_MATCH_OPTIONAL|RPA_MATCH_MULTIPLE))
		return "*";
	else if (((rpa_mnode_t*)cls)->flags & RPA_MATCH_OPTIONAL)
		return "?";
	else if (((rpa_mnode_t*)cls)->flags & RPA_MATCH_MULTIPLE )
		return "+";
	else
		return "x";

	return "";
}


static int rpa_mnode_dump(rpa_class_t *cls, char *buffer, unsigned int size)
{
	return 0;
}


static void rpa_mnode_destroy(rpa_class_t *cls)
{
	rpa_mnode_t *mnode = (rpa_mnode_t*)cls;
	r_free(mnode);
}


static rpa_class_methods_t rpa_mnode_methods = {
	rpa_mnode_getid,
	rpa_mnode_getstr,
	rpa_mnode_dump,
	rpa_mnode_destroy,
};


static rpa_class_methods_t rpa_mnode_callback_methods = {
	rpa_mnode_getid,
	rpa_mnode_getstr,
	rpa_mnode_dump,
	rpa_mnode_destroy,
};


int rpa_mnode_check_for_loop(rpa_mnode_t *mnode, rpa_match_t *loop, rpa_head_t *mhead, int reclevel)
{
	int ret = 0;
	unsigned int classid = 0;
	rpa_match_t *match = mnode->match;
	rpa_dloop_t dloop;
	rpa_link_t *pos;

	if (reclevel > RPA_MAX_RECURSION)
		return 0;
		
//	fprintf(stdout, "%s: %s\n", __FUNCTION__, match->name);
	/* easy case */
	if (reclevel && match == loop) {
		mnode->flags |= RPA_MNODE_LOOP;
		return 1;
	}
	
	for (pos = rpa_list_last(mhead); pos; pos = rpa_list_prev(mhead, pos)) {
		rpa_dloop_t *pLoop = rpa_list_entry(pos, rpa_dloop_t, lnk);
		if (pLoop->match == match)
			return 0;
	}	

	rpa_list_init(&dloop.lnk);
	dloop.match = match;
	rpa_list_addt(mhead, &dloop.lnk);

	classid = rpa_class_getid((rpa_class_t *)mnode->match);
	if (classid & (RPA_MATCH_LIST_CLASSID|RPA_MATCH_NLIST_CLASSID)) {
		rpa_mnode_t *hcur;
		rpa_link_t *pos;
		rpa_head_t *head = &((rpa_match_list_t *)match)->head;
		switch (match->match_function_id) {
			case RPA_MATCHFUNC_LIST_ALT:
			case RPA_MATCHFUNC_NLIST_ALT:
			case RPA_MATCHFUNC_NLIST_BESTALT:
				rpa_list_for_each(pos, head) {
					hcur = rpa_list_entry(pos, rpa_mnode_t, mlink);
					if (rpa_mnode_check_for_loop(hcur, loop, mhead, reclevel + 1))
						ret = 1;
				}
			break;
			case RPA_MATCHFUNC_LIST:
				rpa_list_for_each(pos, head) {
					hcur = rpa_list_entry(pos, rpa_mnode_t, mlink);
					if (rpa_mnode_check_for_loop(hcur, loop, mhead, reclevel + 1))
						ret = 1;
					if ((hcur->flags & RPA_MATCH_OPTIONAL) == 0)
						break;
				}
			break;
			default:
				ret = 0;
			break;
		}
	}
	
	rpa_list_del(&dloop.lnk);

	if (ret)
		mnode->flags |= RPA_MNODE_LOOP;	
	return ret;
}


rpa_mnode_t *rpa_mnode_init(
	rpa_mnode_t *mnode,
	rpa_match_t *match, 
	unsigned int flags,
	rpa_class_methods_t *vptr)
{
	r_memset(mnode, 0, sizeof(*mnode));
	rpa_class_init((rpa_class_t*)mnode, vptr);
	mnode->match = match;
	mnode->flags = flags;
	rpa_list_init(&mnode->mlink);
	return mnode;
}


rpa_mnode_t *rpa_mnode_callback_init(
	rpa_mnode_callback_t *mnode,
	rpa_match_t *match, 
	unsigned int flags,
	RPA_MATCH_CALLBACK matched_callback, 
	void *userdata,
	rpa_class_methods_t *vptr)
{
	flags |= RPA_MNODE_CALLBACK; 
	rpa_mnode_init((rpa_mnode_t*)mnode, match, flags, vptr);
	mnode->matched_callback = matched_callback;
	mnode->userdata = userdata;
	return (rpa_mnode_t*)mnode;
}

rpa_mnode_t *rpa_mnode_create(
	rpa_match_t *match, 
	unsigned int flags)
{
	rpa_mnode_t *newnode;

	newnode = (rpa_mnode_t*) r_malloc(sizeof(*newnode));
	if (!newnode)
		return ((void*)0);
	return rpa_mnode_init(newnode, match, flags, &rpa_mnode_methods);
}


rpa_mnode_t *rpa_mnode_callback_create(
	rpa_match_t *match, 
	unsigned int flags, 
	RPA_MATCH_CALLBACK matched_callback, 
	void *userdata)
{
	rpa_mnode_callback_t *newnode;

	newnode = (rpa_mnode_callback_t*) r_malloc(sizeof(*newnode));
	if (!newnode)
		return ((void*)0);
	return rpa_mnode_callback_init(newnode, match, flags, matched_callback, userdata, &rpa_mnode_callback_methods);
}


rpa_mnode_t *rpa_mnode_callback_arg1_create(
	rpa_match_t *match, 
	unsigned int flags, 
	RPA_MATCH_CALLBACK matched_callback, 
	void *userdata,
	rpa_word_t arg1)
{
	rpa_mnode_callback_arg1_t *newnode;

	newnode = (rpa_mnode_callback_arg1_t*) r_malloc(sizeof(*newnode));
	if (!newnode)
		return ((void*)0);
	newnode->arg1 = arg1;
	return rpa_mnode_callback_init((rpa_mnode_callback_t*)newnode, match, flags, matched_callback, userdata, &rpa_mnode_callback_methods);
	
}


rpa_mnode_t *rpa_mnode_callback_arg2_create(
	rpa_match_t *match, 
	unsigned int flags, 
	RPA_MATCH_CALLBACK matched_callback, 
	void *userdata,
	rpa_word_t arg1,
	rpa_word_t arg2)
{
	rpa_mnode_callback_arg2_t *newnode;

	newnode = (rpa_mnode_callback_arg2_t*) r_malloc(sizeof(*newnode));
	if (!newnode)
		return ((void*)0);
	newnode->base.arg1 = arg1;
	newnode->arg2 = arg2;
	return rpa_mnode_callback_init((rpa_mnode_callback_t*)newnode, match, flags, matched_callback, userdata, &rpa_mnode_callback_methods);
}


rpa_mnode_t *rpa_mnode_list_append(rpa_head_t *head, rpa_mnode_t *mnode)
{
	rpa_list_addt(head, &mnode->mlink);
	return mnode;
}


void rpa_mnode_cat_list(rpa_head_t *head, rpa_mnode_t *first, ...)
{
	va_list args;
	rpa_mnode_t *next;

	va_start(args, first);
	for (next = first; next; next = va_arg(args, rpa_mnode_t*)) {
		rpa_mnode_list_append(head, next);
	}
	va_end(args);
}


int rpa_mnode_exec_callback(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input, unsigned int size, unsigned int reason)
{
	int ret = size;

	if (input >= stat->end)
		return -1;
	if ( ((rpa_mnode_callback_t*)mnode)->matched_callback && (reason & mnode->flags))
		ret = ((rpa_mnode_callback_t*)mnode)->matched_callback(mnode, stat, input, size, (reason & mnode->flags));
	return ret;
}


int rpa_mnode_record_callback(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input, unsigned int size, unsigned int reason)
{
	rpa_cbrecord_t *cbrec;

	if (input >= stat->end)
		return -1;
	if (mnode->flags & RPA_MNODE_SYNCRONOUS)
		return rpa_mnode_exec_callback(mnode, stat, input, size, reason);
	if ( ((rpa_mnode_callback_t*)mnode)->matched_callback) {
		if ((cbrec = rpa_cbset_push(&stat->cbset)) != 0) {
			cbrec->mnode = mnode;
			cbrec->input = input;
			cbrec->size = size;
		}
	}
	return size;
}


int rpa_mnode_plain(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;
	rpa_match_t *match = mnode->match;

	if (stat->currentloop && stat->currentloop->size && stat->currentloop->input == input && !(mnode->flags & RPA_MNODE_LOOP))
		return 0;

	ret = stat->mtable[match->match_function_id](mnode->match, stat, input);
	if (ret <= 0)
		return -1;
	return ret;
}


int rpa_mnode_optional(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;
	ret = rpa_mnode_plain(mnode, stat, input);
	if (ret < 0)
		return 0;
	return ret;
}


int rpa_mnode_multiple(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret = 0, mret;
	
	mret = rpa_mnode_plain(mnode, stat, input);
	if (mret < 0)
		return -1;
	ret += mret;
	while ((mret = rpa_mnode_optional(mnode, stat, input + ret))) {
		ret += mret;
	}
	return ret;
}


int rpa_mnode_multiopt(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret = 0, mret;
	
	while ((mret = rpa_mnode_optional(mnode, stat, input + ret))) {
		ret += mret;
	}
	return ret;
}



int rpa_mnode_plain_loop_detect(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;
	rpa_dloop_t *pLoop;
	rpa_match_t *match = mnode->match;
	rpa_dloop_t loop = RPA_DLOOP_INIT(loop, match, input);
	rpa_head_t *bucket = &stat->loophash[RPA_LOOPHASH(match)];
	rpa_link_t *pos;

	pLoop = rpa_stat_current_loop(stat);
	if (pLoop && pLoop->match == mnode->match) {
		return pLoop->size;
	}

	for (pos = rpa_list_last(bucket); pos; pos = rpa_list_prev(bucket, pos)) {
		pLoop = rpa_list_entry(pos, rpa_dloop_t, lnk);
		if (pLoop->match == match && pLoop->input == input) {
			pLoop->mnode = mnode;
			rpa_list_del(pos);
			rpa_list_addt(&stat->loopstack, pos);
			stat->currentloop = pLoop;
			return pLoop->size;
		}
	}

	rpa_list_addt(bucket, &loop.lnk);
	ret = stat->mtable[match->match_function_id](mnode->match, stat, input);
	if (loop.mnode) {
		while (ret > loop.size) {
			loop.size = ret;
			ret = stat->mtable[match->match_function_id](mnode->match, stat, input);
		}
	}
	rpa_list_del(&loop.lnk);
	stat->currentloop = rpa_stat_current_loop(stat);
	if (loop.size)
		ret = loop.size;
	if (ret <= 0)
		return -1;
	return ret;
}


int rpa_mnode_callback_plain(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;
	rpa_match_t *match = mnode->match;
	rpa_mcache_t *mcache = &stat->mcache[RPA_MCACHEHASH(match)];

	rpa_mnode_exec_callback(mnode, stat, input, (unsigned int) (stat->end - input), RPA_REASON_START);
	if (((rpa_match_nlist_t*)(mnode->match))->loopy) {
		ret = rpa_mnode_plain_loop_detect(mnode, stat, input);
	} else if (stat->usecache && mcache->match == match && mcache->input == input) {
		ret = mcache->ret;
		/*
		 * Debug the cache efficiency
		 * r_printf("HIT THE CACHE@%d: %s, %d\n", RPA_MCACHEHASH(match), match->name, ret);
		 */
	} else {
		ret = rpa_mnode_plain(mnode, stat, input);
	}
	if (ret > 0)
		RPA_MCACHE_SET(mcache, match, input, ret);
	if (ret <= 0) {
		rpa_mnode_exec_callback(mnode, stat, input, 0, RPA_REASON_END);
		return -1;
	}
	ret = rpa_mnode_exec_callback(mnode, stat, input, ret, RPA_REASON_END|RPA_REASON_MATCHED);
	if (!ret) 
		return -1;
	return ret;
}


int rpa_mnode_callback_optional(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;

	ret = rpa_mnode_callback_plain(mnode, stat, input);
	if (ret < 0) 
		return 0;
	
	return ret;
}


int rpa_mnode_callback_multiple(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret = 0, mret;

	mret = rpa_mnode_callback_plain(mnode, stat, input);
	if (mret <= 0) 
		return mret;

	ret += mret;
	do {
		mret = rpa_mnode_callback_optional(mnode, stat, input + ret);
		ret += mret;
	} while (mret);

	return ret;
}


int rpa_mnode_callback_multiopt(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret = 0, mret;

	do {
		mret = rpa_mnode_callback_optional(mnode, stat, input + ret);
		ret += mret;
	} while (mret);
	
	return ret;
}



/* Parser functionality */

int rpa_mnode_p_plain_loop_detect(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;
	rpa_word_t off;	
	rpa_dloop_t *pLoop;
	rpa_match_t *match = mnode->match;
	rpa_dloop_t loop = RPA_DLOOP_INIT(loop, match, input);
	rpa_head_t *bucket = &stat->loophash[RPA_LOOPHASH(match)];
	rpa_link_t *pos;

	pLoop = rpa_stat_current_loop(stat);
	if (pLoop && pLoop->match == mnode->match) {
		return pLoop->size;
	}

	for (pos = rpa_list_last(bucket); pos; pos = rpa_list_prev(bucket, pos)) {
		pLoop = rpa_list_entry(pos, rpa_dloop_t, lnk);
		if (pLoop->match == match && pLoop->input == input) {
			pLoop->mnode = mnode;
			rpa_list_del(pos);
			rpa_list_addt(&stat->loopstack, pos);
			stat->currentloop = pLoop;
			return pLoop->size;
		}
	}

	rpa_list_addt(bucket, &loop.lnk);
	off = rpa_cbset_getpos(&stat->cbset);
	ret = stat->mtable[match->match_function_id](mnode->match, stat, input);
	if (ret <= 0)
		rpa_cbset_reset(&stat->cbset, off);
	if (loop.mnode) {
		while (ret > loop.size) {
			loop.size = ret;
			off = rpa_cbset_getpos(&stat->cbset);
			ret = stat->mtable[match->match_function_id](mnode->match, stat, input);
			if (ret <= loop.size)
				rpa_cbset_reset(&stat->cbset, off);
		}
	}
	rpa_list_del(&loop.lnk);
	stat->currentloop = rpa_stat_current_loop(stat);
	if (loop.size)
		ret = loop.size;
	if (ret <= 0)
		return -1;

	return ret;
}



int rpa_mnode_p_plain(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;
	rpa_word_t off = rpa_cbset_getpos(&stat->cbset);
	rpa_match_t *match = mnode->match;

	if (stat->currentloop && stat->currentloop->size && stat->currentloop->input == input && !(mnode->flags & RPA_MNODE_LOOP))
		return 0;
	ret = stat->mtable[match->match_function_id](mnode->match, stat, input);
	if (ret <= 0) {
		rpa_cbset_reset(&stat->cbset, off);
		return -1;
	}
	return ret;
}


int rpa_mnode_p_optional(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;
	ret = rpa_mnode_p_plain(mnode, stat, input);
	if (ret < 0)
		return 0;
	return ret;
}


int rpa_mnode_p_multiple(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret = 0, mret;
	
	mret = rpa_mnode_p_plain(mnode, stat, input);
	if (mret < 0)
		return -1;
	ret += mret;
	while ((mret = rpa_mnode_p_optional(mnode, stat, input + ret))) {
		ret += mret;
	}
	return ret;
}


int rpa_mnode_p_multiopt(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret = 0, mret;
	
	while ((mret = rpa_mnode_p_optional(mnode, stat, input + ret))) {
		ret += mret;
	}
	return ret;
}


int rpa_mnode_p_callback_plain(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;
	rpa_match_t *match = mnode->match;
	rpa_mcache_t *mcache = &stat->mcache[RPA_MCACHEHASH(match)];
	rpa_word_t cboffset;

	if (((rpa_match_nlist_t*)match)->loopy) {
		ret = rpa_mnode_p_plain_loop_detect(mnode, stat, input);
	} else if (mcache->match == match && mcache->input == input) {
		/*
		 * We can get here only from cache entries that point to
		 * valid cbset positions. We can only get here if ret > 0
		 * so reset the cbset position.
		 */
		rpa_cbset_reset(&stat->cbset, mcache->cboffset);
		ret = mcache->ret;
		/*
		 * Debug the cache efficiency
		 * r_printf("HIT THE CACHE @ %d: %s, %d\n", RPA_MCACHEHASH(match), match->name, ret);
		 */
	} else {
		ret = rpa_mnode_p_plain(mnode, stat, input);
	}

	if (ret > 0) {
		/*
		 * Get the cboffset before eventually record on the cbset stack.
		 */
		cboffset = rpa_cbset_getpos(&stat->cbset);
		ret = rpa_mnode_record_callback(mnode, stat, input, ret, RPA_REASON_START|RPA_REASON_END|RPA_REASON_MATCHED);
		/*
		 * All cache entries that point to cbset positions higher than the current position
		 * need to be invalidated.
		 */
		rpa_stat_cache_cbreset(stat, rpa_cbset_getpos(&stat->cbset));
		if (!ret)
			return -1;
		/*
		 * Set the cache. All previous entries for the current position of the cbset must be removed
		 * from the cache with the call to rpa_stat_cache_cbreset().
		 */
		RPA_MCACHE_CBSET(mcache, match, input, ret, cboffset);
	}
	
	if (ret <= 0) {
		return -1;
	}
	return ret;
}


int rpa_mnode_p_callback_optional(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret;

	ret = rpa_mnode_p_callback_plain(mnode, stat, input);
	if (ret < 0) 
		return 0;
	
	return ret;
}


int rpa_mnode_p_callback_multiple(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret = 0, mret;

	mret = rpa_mnode_p_callback_plain(mnode, stat, input);
	if (mret <= 0) 
		return mret;

	ret += mret;
	do {
		mret = rpa_mnode_p_callback_optional(mnode, stat, input + ret);
		ret += mret;
	} while (mret);

	return ret;
}


int rpa_mnode_p_callback_multiopt(rpa_mnode_t *mnode, rpa_stat_t *stat, const char *input)
{
	int ret = 0, mret;

	do {
		mret = rpa_mnode_p_callback_optional(mnode, stat, input + ret);
		ret += mret;
	} while (mret);
	
	return ret;
}
