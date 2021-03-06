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

#include "rlib/rarray.h"
#include "rlib/rmem.h"


#define MIN_ARRAY_LEN 2

static void r_array_checkexpand(rarray_t *array, unsigned long index);


void r_array_cleanup(robject_t *obj)
{
	rarray_t *array = (rarray_t *)obj;
	if (array->oncleanup)
		array->oncleanup(array);
	r_free(array->data);
	r_object_cleanup((robject_t*)array);
}


robject_t *r_array_init(robject_t *obj, ruint32 type, r_object_cleanupfun cleanup, r_object_copyfun copy, unsigned long elt_size)
{
	rarray_t *array = (rarray_t*)obj;

	r_object_init(obj, type, cleanup, copy);
	array->elt_size = elt_size;
	array->data = r_zmalloc(MIN_ARRAY_LEN * array->elt_size);
	array->alloc_size = MIN_ARRAY_LEN;
	return obj;
}


rarray_t *r_array_create(unsigned long elt_size)
{
	rarray_t *array;
	array = (rarray_t*)r_object_create(sizeof(*array));
	r_array_init((robject_t*)array, R_OBJECT_ARRAY, r_array_cleanup, r_array_copy, elt_size);
	return array;
}


void r_array_destroy(rarray_t *array)
{
	r_object_destroy((robject_t*)array);
}


robject_t *r_array_copy(const robject_t *obj)
{
	unsigned long i;
	rarray_t *dst;
	const rarray_t *array = (const rarray_t *)obj;

	if (!array)
		return NULL;
	dst = r_array_create(array->elt_size);
	if (!dst)
		return NULL;
	for (i = 0; i < r_array_length(array); i++)
		r_array_replace(dst, i, r_array_slot(array, i));
	dst->oncopy = array->oncopy;
	dst->oncleanup = array->oncleanup;
	if (dst->oncopy)
		dst->oncopy(dst);
	return (robject_t *)dst;
}


static void r_array_exist_replace(rarray_t *array, unsigned long index, rconstpointer data)
{
	if (data)
		r_memcpy(r_array_slot(array, index), data, array->elt_size);
	else
		r_memset(r_array_slot(array, index), 0, array->elt_size);
}


unsigned long r_array_add(rarray_t *array, rconstpointer data)
{
	unsigned long index = r_array_length(array);

	r_array_setlength(array, index + 1);
	r_array_exist_replace(array, index, data);
	return index;
}


unsigned long r_array_removelast(rarray_t *array)
{
	if (!r_array_empty(array))
		r_array_setlength(array, r_array_length(array) - 1);
	return r_array_length(array);
}


unsigned long r_array_insert(rarray_t *array, unsigned long index, rconstpointer data)
{
	r_array_checkexpand(array, index + 1);
	if (index < r_array_length(array)) {
		unsigned long curlen = r_array_length(array);
		r_array_setlength(array, r_array_length(array) + 1);
		r_memmove(r_array_slot(array, index + 1), r_array_slot(array, index), (curlen - index) * array->elt_size);
	} else {
		r_array_setlength(array, index + 1);
	}
	r_array_exist_replace(array, index, data);
	return index;
}


unsigned long r_array_replace(rarray_t *array, unsigned long index, rconstpointer data)
{
	if (index >= r_array_length(array))
		return r_array_insert(array, index, data);
	r_array_exist_replace(array, index, data);
	return index;
}


unsigned long r_array_setlength(rarray_t *array, unsigned long len)
{
	r_array_checkexpand(array, len);
	r_array_length(array) = len;
	return r_array_length(array);
}


void r_array_expand(rarray_t *array, unsigned long len)
{
	r_array_checkexpand(array, r_array_length(array) + len);
}


static void r_array_checkexpand(rarray_t *array, unsigned long len)
{
	unsigned long nalloc_size;
	rpointer data;

	if (array->alloc_size < len) {
		for (nalloc_size = array->alloc_size; nalloc_size < len;)
			nalloc_size = 2 * nalloc_size + 1;
		data = r_realloc(array->data, nalloc_size * array->elt_size);
		if (data) {
			unsigned long old_len = array->alloc_size;
			array->data = data;
			array->alloc_size = nalloc_size;

			/*
			 * Zero the newly allocated memory - only the extension (above the alloc_size).
			 */
			r_memset((void*)r_array_slot(array, old_len), 0, (array->alloc_size - old_len) * array->elt_size);
		}
	}
}


void *r_array_slot_expand(rarray_t *array, unsigned long index)
{
	r_array_checkexpand(array, index+1);
	return (void*) r_array_slot(array, index);
}


int r_array_move(rarray_t *array, unsigned long dst, unsigned long src, unsigned long size)
{
	long i;
	if ((dst + size) > r_array_length(array))
		return -1;
	if (dst == src)
		return 0;
	if (dst < src) {
		for (i = 0; i < size; i++) {
			r_array_replace(array, dst + i, r_array_slot(array, src + i));
		}
	} else {
		for (i = size; i > 0; i--) {
			r_array_replace(array, dst + i - 1, r_array_slot(array, src + i - 1));
		}
	}
	return 0;
}


void r_array_delete(rarray_t *array, unsigned long index)
{
	unsigned long movesize;
	unsigned long moveindex;

	if (r_array_length(array) <= index)
		return;
	moveindex = index + 1;
	movesize = r_array_length(array) - moveindex;
	if (movesize > 0) {
		r_array_move(array, index, moveindex, movesize);
	}
	r_array_removelast(array);
}
