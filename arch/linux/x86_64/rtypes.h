#ifndef _RTYPES_H_
#define _RTYPES_H_

/*
 * Common types. These types should be the same for most of the architectures.
 */
typedef long rlong;
typedef int rint;
typedef short rshort;
typedef char rchar;
typedef unsigned long rulong;
typedef unsigned int ruint;
typedef unsigned short rushort;
typedef unsigned char ruchar;
typedef double rdouble;
typedef float rfloat;
typedef rint rboolean;
typedef void *rpointer;
typedef const void *rconstpointer;


/* 
 * Architecture dependent types. These types have to be redifined 
 * for every architecture
 */
typedef signed char rint8;
typedef unsigned char ruint8;
typedef signed short rint16;
typedef unsigned short ruint16;
typedef signed int rint32;
typedef unsigned int ruint32;
typedef signed long rint64;
typedef unsigned long ruint64;
typedef unsigned long rword;


#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((rpointer)0)
#endif
#endif

#endif

