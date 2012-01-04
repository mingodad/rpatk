#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rlib/rmem.h"
#include "rex/rexcompiler.h"

enum {
	REX_TOKEN_ERR = -1,
	REX_TOKEN_EOF = -2,
	REX_TOKEN_SPACE = -4,
	REX_TOKEN_NEWLINE = -5,

};

rexcompiler_t *rex_compiler_create(rexdb_t *db)
{
	rexcompiler_t *co;

	co = (rexcompiler_t *)r_malloc(sizeof(*co));
	r_memset(co, 0, sizeof(*co));
	co->db = db;
	co->stack = r_array_create(sizeof(rexfragment_t*));
	return co;
}


void rex_compiler_destroy(rexcompiler_t *co)
{
	if (co) {
		r_array_destroy(co->stack);
		r_free(co);
	}

}


static int rex_compiler_isspace(int c)
{
	return isspace(c);
}


static int rex_compiler_getchar(rexcompiler_t *co)
{
	int c = REX_TOKEN_EOF;

	if (co->ptr < co->end)
		c = *co->ptr++;
	return c;
}


static int rex_compiler_peekchar(rexcompiler_t *co)
{
	int c = REX_TOKEN_EOF;

	if (co->ptr < co->end)
		c = *co->ptr;
	return c;
}


static int rex_compiler_gettok(rexcompiler_t *co)
{
	int c = rex_compiler_getchar(co);

	if (c == REX_TOKEN_EOF) {
		co->tokenstr[0] = '\0';
		co->token = REX_TOKEN_EOF;
		return co->token;
	} else if (c == ' ' || c == '\t') {
		co->tokenstr[0] = c;
		co->tokenstr[1] = '\0';
		co->token = REX_TOKEN_SPACE;
	} else if (c == '\r') {
		co->tokenstr[0] = c;
		co->tokenstr[1] = '\0';
		if (rex_compiler_peekchar(co) == '\n') {
			c = rex_compiler_getchar(co);
			co->tokenstr[1] = c;
			co->tokenstr[2] = '\0';
		}
		co->token = REX_TOKEN_NEWLINE;
	} else if (c == '\n') {
		co->tokenstr[0] = c;
		co->tokenstr[1] = '\0';
		co->token = REX_TOKEN_NEWLINE;
	} else {
		co->tokenstr[0] = c;
		co->tokenstr[1] = '\0';
		co->token = c;
	}
	return co->token;
}


int rex_compiler_charclass(rexcompiler_t *co)
{
	int negative = 0;
	int low;
	int high;

	rex_compiler_gettok(co);
	if (co->token == '^') {
		negative = 1;
		rex_compiler_gettok(co);
	}

	while (1) {
		switch (co->token) {
		case '[':
		case ']':
		case '-':
			return -1;
			break;
		}
		high = low = co->token;
		rex_compiler_gettok(co);
		if (co->token == '-') {
			high = rex_compiler_gettok(co);
			rex_compiler_gettok(co);
		}
		if (low == high) {

		} else {
			/* ragne */
		}
		if (co->token == ']') {
			rex_compiler_gettok(co); /* eat it */
			break;
		}
	}

	return 0;
}


int rex_compiler_factor(rexcompiler_t *co)
{
	if (!strchr("*?+[]()|.", co->token)) {

		rex_compiler_gettok(co);
		return 0;
	} else if (co->token == '[') {
		return rex_compiler_charclass(co);
	}

	return -1;
}


int rex_compiler_qfactor(rexcompiler_t *co)
{
	const char *begin = co->ptr - 1;

	if (rex_compiler_factor(co) < 0)
		return -1;
	while (co->token == REX_TOKEN_SPACE)
		rex_compiler_gettok(co);
	switch (co->token) {
	case '*':
	case '?':
	case '+':

		rex_compiler_gettok(co);
		break;
	}
	if (co->ptr > begin) {
		fprintf(stdout, "qfactor: ");
		fwrite(begin, 1, co->ptr - begin - 1, stdout);
		fprintf(stdout, "\n");
	}
	return 0;
}


int rex_compiler_catexpression(rexcompiler_t *co)
{
	while (1) {
		switch (co->token) {
		case REX_TOKEN_EOF:
		case '|':
			return 0;
		case REX_TOKEN_SPACE:
		case REX_TOKEN_NEWLINE:
			rex_compiler_gettok(co);
			continue;
		}
		if (rex_compiler_qfactor(co) < 0)
			return -1;
		rex_compiler_gettok(co);
	}
	return 0;
}


rexfragment_t *rex_compiler_expression(rexcompiler_t *co, const char *str, unsigned int size, void *accdata)
{
	co->start = str;
	co->end = str + size;
	co->ptr = str;

	rex_compiler_gettok(co);
	while (co->token != REX_TOKEN_EOF) {
		if (co->token == REX_TOKEN_SPACE || co->token == REX_TOKEN_NEWLINE) {
			rex_compiler_gettok(co);
			continue;
		}
		if (rex_compiler_qfactor(co) < 0)
			break;
	}
	return NULL;
}


rexfragment_t *rex_compiler_expression_s(rexcompiler_t *co, const char *str, void *accdata)
{
	return rex_compiler_expression(co, str, r_strlen(str), accdata);
}

