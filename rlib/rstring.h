#ifndef _RSTRING_H_
#define _RSTRING_H_


#include "rtypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct rstring_s {
	rchar *str;
	ruint size;
} rstring_t;


rint r_strcmp(const rchar *s1, const rchar *s2);
rint r_strncmp(const rchar *s1, const rchar *s2, rsize_t n);
rsize_t r_strlen(const rchar *s);
rchar *r_strstr(const rchar *haystack, const rchar *needle);
rchar *r_strdup(const rchar *s);
rchar *r_strndup(const rchar *s, rsize_t size);
rchar *r_strcpy(rchar *dest, const rchar *src);
rchar *r_strncpy(rchar *dest, const rchar *src, rsize_t n);
rstring_t *r_stringdup(const rchar *s, ruint size);


#ifdef __cplusplus
}
#endif

#endif
