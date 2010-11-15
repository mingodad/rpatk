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

#include "rpaposmnodestack.h"
#include "rpamem.h"

rpa_posmnodestack_t *rpa_posmnodestack_create(rpa_word_t initialsize, rpa_word_t grow)
{
	rpa_posmnodestack_t *stack;

	stack = (rpa_posmnodestack_t *)rpa_malloc(sizeof(*stack));
	if (!stack)
		return ((void*)0);
	rpa_memset(stack, 0, sizeof(*stack));
	stack->size = initialsize;
	stack->grow = grow;
	stack->p = stack->buffer = rpa_malloc((unsigned long)(sizeof(rpa_posmnode_t) * initialsize));
	if (!stack->p) {
		rpa_free(stack);
		return (void*)0;
	}
	stack->p->pos = 0;
	stack->p->mnode = 0;
	return stack;
}


void rpa_posmnodestack_destroy(rpa_posmnodestack_t *stack)
{
	rpa_free(stack->buffer);
	rpa_free(stack);	
}


int rpa_posmnodestack_check_space(rpa_posmnodestack_t *stack)
{
	if (stack->size - (stack->p - stack->buffer) < stack->grow) {
		rpa_posmnode_t *buffer;
		rpa_word_t off = stack->p - stack->buffer;
		if ((buffer = rpa_realloc(stack->buffer, (unsigned long)(stack->size + stack->grow) * sizeof(rpa_posmnode_t))) == 0)
			return -1;
		stack->buffer = buffer;
		stack->p = buffer + off;
		stack->size += stack->grow;
	}
	return 0;
}


void rpa_posmnodestack_reset(rpa_posmnodestack_t *stack)
{
	stack->p = stack->buffer;
}


rpa_posmnode_t *rpa_posmnodestack_push(rpa_posmnodestack_t *stack, const char *pos, rpa_mnode_t *mnode)
{
	stack->p++;
	stack->p->pos = pos;
	stack->p->mnode = mnode;
	return stack->p;
}


rpa_posmnode_t *rpa_posmnodestack_pop(rpa_posmnodestack_t *stack)
{
	stack->p--;
	return stack->p;
}
