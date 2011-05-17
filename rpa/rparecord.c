#include "rmem.h"
#include "rstring.h"
#include "rpaparser.h"
#include "rparecord.h"


rparecord_t *rpa_record_get(rarray_t *records, rlong rec)
{
	rparecord_t *prec;

	if (!records)
		return NULL;
	if (rec < 0 || rec >= r_array_length(records))
		return NULL;
	prec = (rparecord_t *)r_array_slot(records, rec);
	return prec;

}


rlong rpa_recordtree_get(rarray_t *records, rlong rec, rulong type)
{
	rlong i, s = 0;
	ruint startrec = (type & RPA_RECORD_START) ? 1 : 0;
	rparecord_t *prec;

	if (rec < 0 || rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if ((prec->type & RPA_RECORD_START)) {
		if (startrec)
			return rec;
		for (s = 0, i = rec; i < r_array_length(records); i++) {
			prec = (rparecord_t *)r_array_slot(records, i);
			if (prec->type & RPA_RECORD_START)
				++s;
			if (prec->type & RPA_RECORD_END)
				--s;
			if (s == 0)
				return i;
		}
	}
	prec = (rparecord_t *)r_array_slot(records, rec);
	if ((prec->type & RPA_RECORD_END)) {
		if (!startrec)
			return rec;
		for (s = 0, i = rec; i >= 0; i--) {
			prec = (rparecord_t *)r_array_slot(records, i);
			if (prec->type & RPA_RECORD_START)
				++s;
			if (prec->type & RPA_RECORD_END)
				--s;
			if (s == 0)
				return i;
		}
	}

	return -1;
}


rlong rpa_recordtree_firstchild(rarray_t *records, rlong rec, rulong type)
{
	rparecord_t *prec;

	if (rec < 0 || rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_END) {
		if ((rec = rpa_recordtree_get(records, rec, RPA_RECORD_START)) < 0)
			return -1;
	}
	if (++rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_START)
		return rpa_recordtree_get(records, rec, type);
	return -1;
}


rlong rpa_recordtree_lastchild(rarray_t *records, rlong rec, rulong type)
{
	rparecord_t *prec;

	if (rec < 0 || rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_START) {
		if ((rec = rpa_recordtree_get(records, rec, RPA_RECORD_END)) < 0)
			return -1;
	}
	if (--rec < 0)
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_END)
		return rpa_recordtree_get(records, rec, type);
	return -1;
}


rlong rpa_recordtree_next(rarray_t *records, rlong rec, rulong type)
{
	rparecord_t *prec;

	if (rec < 0 || rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_START) {
		if ((rec = rpa_recordtree_get(records, rec, RPA_RECORD_END)) < 0)
			return -1;
	}
	if (++rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_START)
		return rpa_recordtree_get(records, rec, type);
	return -1;
}


rlong rpa_recordtree_prev(rarray_t *records, rlong rec, rulong type)
{
	rparecord_t *prec;

	if (rec < 0 || rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_END) {
		if ((rec = rpa_recordtree_get(records, rec, RPA_RECORD_START)) < 0)
			return -1;
	}
	if (--rec < 0)
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_END)
		return rpa_recordtree_get(records, rec, type);
	return -1;
}


rlong rpa_recordtree_parent(rarray_t *records, rlong rec, rulong type)
{
	rlong last = -1, parent = -1;

	if (rec < 0 || rec >= r_array_length(records))
		return -1;
	for ( ;rec >= 0; rec = rpa_recordtree_next(records, last, RPA_RECORD_END)) {
		last = rpa_recordtree_get(records, rec, RPA_RECORD_END);
	}
	parent = last + 1;
	if (parent >= r_array_length(records))
		return -1;
	return rpa_recordtree_get(records, parent, type);
}


rlong rpa_recordtree_size(rarray_t *records, rlong rec)
{
	rlong first = rpa_recordtree_get(records, rec, RPA_RECORD_START);
	rlong last = rpa_recordtree_get(records, rec, RPA_RECORD_END);
	if (first < 0 || last < 0)
		return -1;
	return (last - first + 1);
}


rlong rpa_recordtree_rotatedown(rarray_t *records, rlong parent)
{
	rlong nlastchild;
	rlong nallrecs = r_array_length(records);
	rlong firstchild = rpa_recordtree_firstchild(records, parent, RPA_RECORD_START);
	rlong lastchild = rpa_recordtree_lastchild(records, parent, RPA_RECORD_START);
	rlong copysiz;

	if (firstchild < 0 || lastchild == firstchild)
		return -1;
	/*
	 * Copy the last child at the end
	 */
	nlastchild = rpa_recordtree_size(records, lastchild);
	r_array_move(records, nallrecs, lastchild, nlastchild);

	/*
	 * Push down all children, but the last one (overwriting the last one)
	 */
	copysiz = lastchild - firstchild;
	r_array_move(records, firstchild + nlastchild, firstchild, copysiz);

	/*
	 * Copy the last child to become the first one
	 */
	r_array_move(records, firstchild, nallrecs, nlastchild);

	/*
	 * Restore the original size
	 */
	r_array_setlength(records, nallrecs);
	return 0;
}


rlong rpa_recordtree_rotateup(rarray_t *records, rlong parent)
{
	/*
	 * Not implemented
	 */
	return -1;
}


rlong rpa_recordtree_copy(rarray_t *dst, rarray_t *src, rlong rec)
{
	rparecord_t *prec;
	rlong size, i;
	rec = rpa_recordtree_get(src, rec, RPA_RECORD_START);
	size = rpa_recordtree_size(src, rec);

	for (i = 0; i < size; i++) {
		prec = rpa_record_get(src, i);
		r_array_add(dst, prec);
	}
	return size;
}


rlong rpa_recordtree_walk(rarray_t *records, rlong rec, rlong level, rpa_recordtree_callback callback, rpointer userdata)
{
	rlong child;

	if (level > 128)
		return -1;
	rec = rpa_recordtree_get(records, rec, RPA_RECORD_START);
	if (callback && callback(records, rec, userdata) < 0)
		return -1;
	for (child = rpa_recordtree_firstchild(records, rec, RPA_RECORD_START); child >= 0; child = rpa_recordtree_next(records, child, RPA_RECORD_START)) {
		if (rpa_recordtree_walk(records, child, level + 1, callback, userdata) < 0)
			return -1;
	}
	rec = rpa_recordtree_get(records, rec, RPA_RECORD_END);
	if (callback && callback(records, rec, userdata) < 0)
		return -1;
	return 0;
}


static void rpa_recordptr_setusertype(rparecord_t *prec, ruint32 usertype, rvalset_t op)
{
	switch (op) {
	case RVALSET_OR:
		prec->usertype |= usertype;
		break;
	case RVALSET_XOR:
		prec->usertype ^= usertype;
		break;
	case RVALSET_AND:
		prec->usertype &= usertype;
		break;
	default:
		prec->usertype = usertype;
	}
}


rlong rpa_record_getruleuid(rarray_t *records, rlong rec)
{
	rparecord_t *prec;

	if (rec < 0)
		return -1;
	rec = rpa_recordtree_get(records, rec, RPA_RECORD_START);
	if (rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	return prec->ruleuid;
}


void rpa_record_setusertype(rarray_t *records, rlong rec, ruint32 usertype, rvalset_t op)
{
	rparecord_t *prec;

	if (rec < 0)
		return;
	rec = rpa_recordtree_get(records, rec, RPA_RECORD_START);
	if (rec >= r_array_length(records))
		return;
	prec = (rparecord_t *)r_array_slot(records, rec);
	rpa_recordptr_setusertype(prec, usertype, op);
	rec = rpa_recordtree_get(records, rec, RPA_RECORD_END);
	if (rec >= r_array_length(records))
		return;
	prec = (rparecord_t *)r_array_slot(records, rec);
	rpa_recordptr_setusertype(prec, usertype, op);
}


rlong rpa_record_getusertype(rarray_t *records, rlong rec)
{
	rparecord_t *prec;

	if (rec < 0)
		return -1;
	rec = rpa_recordtree_get(records, rec, RPA_RECORD_START);
	if (rec >= r_array_length(records))
		return -1;
	prec = (rparecord_t *)r_array_slot(records, rec);
	return prec->usertype;
}


rint rpa_record_optchar(rparecord_t *prec, rint defc)
{
	rint optc = defc;

	if ((prec->usertype & RPA_MATCH_MASK) == RPA_MATCH_OPTIONAL)
		optc = '?';
	else if ((prec->usertype & RPA_MATCH_MASK) == RPA_MATCH_MULTIPLE)
		optc = '+';
	else if ((prec->usertype & RPA_MATCH_MASK) == RPA_MATCH_MULTIOPT)
		optc = '*';
	else
		optc = defc;
	return optc;
}


rint rpa_record_loopchar(rparecord_t *prec, rint defc)
{
	rint loopc = defc;

	if ((prec->usertype & RPA_LOOP_PATH) && (prec->usertype & RPA_NONLOOP_PATH)) {
		/*
		 * This is an error, should never happen
		 */
		loopc = 'R';
	} else if ((prec->usertype & RPA_LOOP_PATH)) {
		loopc = 'L';
	} else if ((prec->usertype & RPA_NONLOOP_PATH)) {
		loopc = 'N';
	} else {
		loopc = defc;
	}
	return loopc;
}


void rpa_record_dump(rarray_t *records, rlong rec)
{
	rparecord_t *prec;
	rlong start, end, first, last, next, prev, parent;
	rchar buf[240];
	rint bufsize = sizeof(buf) - 1;
	rint n = 0, size;
	rchar optc = ' ';

	if (rec < 0 || rec >= r_array_length(records))
		return;
	prec = (rparecord_t *)r_array_slot(records, rec);
	if (prec->type & RPA_RECORD_END) {
		if ((prec->usertype & RPA_MATCH_MASK) == RPA_MATCH_OPTIONAL)
			optc = '?';
		else if ((prec->usertype & RPA_MATCH_MASK) == RPA_MATCH_MULTIPLE)
			optc = '+';
		else if ((prec->usertype & RPA_MATCH_MASK) == RPA_MATCH_MULTIOPT)
			optc = '*';
	}

	r_memset(buf, 0, bufsize);

	start = rpa_recordtree_get(records, rec, RPA_RECORD_START);
	end = rpa_recordtree_get(records, rec, RPA_RECORD_END);
	first = rpa_recordtree_firstchild(records, rec, RPA_RECORD_START);
	last = rpa_recordtree_lastchild(records, rec, RPA_RECORD_START);
	next = rpa_recordtree_next(records, rec, RPA_RECORD_START);
	prev = rpa_recordtree_prev(records, rec, RPA_RECORD_START);
	parent = rpa_recordtree_parent(records, rec, RPA_RECORD_START);

	n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, "%5ld: [ s: %5ld, e: %5ld, p: %5ld ] ( %4d, 0x%03x ) : ", rec, start, end, parent, prec->ruleuid, prec->usertype);
	if (prec->type & RPA_RECORD_START)
		n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, "START ");
	if (prec->type & RPA_RECORD_END)
		n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, "END   ");
	if (prec->type & RPA_RECORD_HEAD)
		n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, "HEAD ");
	if (prec->type & RPA_RECORD_TAIL)
		n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, "TAIL ");
	n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, "%s ", prec->rule);

	r_memset(buf + n, ' ', bufsize - n);
	n = 115;
	n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, " %5d, %3d", prec->top, prec->size);

	r_memset(buf + n, ' ', bufsize - n);
	n = 130;
	n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, " %c %c %c", optc,
			(prec->usertype & RPA_LOOP_PATH) ? 'L' : ' ', (prec->usertype & RPA_NONLOOP_PATH) ? 'N' : ' ');
	n += r_snprintf(buf + n, n < bufsize ? bufsize - n : 0, " : ");
	size = prec->inputsiz;
	if (size >= bufsize - n - 1)
		size = bufsize - n - 1;
	if (prec->type & RPA_RECORD_END) {
		r_strncpy(buf + n, prec->input, size);
		n += size;
		buf[n] = '\0';
	}

	r_printf("%s\n", buf);
}


void rpa_record_dumpindented(rarray_t *records, rlong rec, rint level)
{
	rchar buffer[1024];
	rparecord_t *prec;
	rint i, size;

	if (rec < 0 || rec >= r_array_length(records))
		return;
	r_memset(buffer, 0, sizeof(buffer));
	prec = (rparecord_t *)r_array_slot(records, rec);
	for (i = 0; i < level; i++)
		r_printf("   ");
	r_printf("   ");
	r_printf("(");
	r_printf("%s, %c, %c", prec->rule, rpa_record_optchar(prec, 'x'), rpa_record_loopchar(prec, 'x'));
	r_printf(")");
	rec = rpa_recordtree_get(records, rec, RPA_RECORD_END);
	prec = (rparecord_t *)r_array_slot(records, rec);
	size = R_MIN(prec->inputsiz, sizeof(buffer) - 1);
	r_strncpy(buffer, prec->input, size);

	if (size == (sizeof(buffer) - 1))
		r_printf(" %s ...\n", buffer);
	else
		r_printf(" %s\n", buffer);
	return;
}