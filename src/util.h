//util.h - vector & string data structure for C
//Copyright (C) 2021  Ayman Wagih Mohsen
//
//This program is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef UTIL_H
#define UTIL_H
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>
#include<assert.h>
#ifdef _MSC_VER
#define	scanf	scanf_s
#endif
#ifdef __cplusplus
extern "C"
{
#endif

//CVec: vector for C
//interface:
//	cv_ctor			cv_begin		cv_size					cv_insert1		cv_erase1		cv_str_push_back
//	cv_ctor_zero	cv_end			cv_at	cv_cat			cv_insert		cv_erase		cv_str_pop_back
//	cv_ctor_copy	cv_advance		cv_pat	cv_cpat			cv_insert_zero					cv_str_append
//	cv_ctor_move	cv_get			cv_push_back			cv_assign						cv_str_cutoff
//	cv_ctor_data	cv_back			cv_pop_back				cv_assign_zero
//	cv_dtor			cv_data									cv_replace

typedef struct _CVHead
{
	size_t count, esize;
	char data[];
} CVHead, *CVec;

static void cv_check_idx(const CVec vec, size_t idx, int insert)
{
	int k;
	if(idx>=vec->count+insert)
	{
		printf(
			"\n\nRUNTIME ERROR\n"
			"vector count = %d, index = %d\n"
			"(pointer = %p, esize = %d bytes)\n\n",
			vec->count, idx, vec, vec->esize);
		scanf("%d", &k);
		abort();
	}
}

//POD-only, shallow operations, NEVER PASS ARGS WITH SIDE-EFFECTS
#define	cv_size(CVEC)				(CVEC)->count
#define	cv_bytes(CVEC)				((CVEC)->count*(CVEC)->esize)
#define	cv_offset(CVEC, IDX)		((IDX)*(CVEC)->esize)
#define	cv_data(TYPE, CVEC)			(TYPE*)(CVEC)->data
static void cv_realloc(CVec *pvec, const char *err_msg, ...)
{
	int k;
	void *newpointer;
	va_list args;

	newpointer=realloc(*pvec, sizeof(CVHead)+cv_bytes(*pvec));
	if(!newpointer)
	{
		va_start(args, err_msg);
		vprintf(err_msg, args);
		va_end(args);
		scanf("%d", &k);
		abort();
	}
	*pvec=(CVec)newpointer;
}

#define	cv_ctor(TYPE, CVEC)						CVEC=(CVec)malloc(sizeof(CVHead)), (CVEC)->count=0, (CVEC)->esize=sizeof(TYPE)
#define	cv_ctor_zero(TYPE, CVEC, COUNT)			CVEC=(CVec)malloc(sizeof(CVHead)+(COUNT)*sizeof(TYPE)), (CVEC)->count=COUNT, (CVEC)->esize=sizeof(TYPE), memset((CVEC)->data, 0, cv_bytes(CVEC))
#define	cv_ctor_copy(CVDST, CVSRC)				CVDST=(CVec)malloc(sizeof(CVHead)+cv_bytes(CVSRC)), memcpy(CVDST, CVSRC, sizeof(CVHead)+cv_bytes(CVSRC))
#define	cv_ctor_move(CVDST, CVSRC)				CVDST=CVSRC, CVSRC=0
#define	cv_ctor_data(TYPE, CVEC, DATA, COUNT)	CVEC=(CVec)malloc(sizeof(CVHead)+(COUNT)*sizeof(TYPE)), (CVEC)->count=COUNT, (CVEC)->esize=sizeof(TYPE), memcpy((CVEC)->data, DATA, cv_bytes(CVEC))

#define	cv_dtor(CVEC)							free(CVEC), CVEC=0//shallow

static void cv_assign_v(CVec *pvec, size_t count, const void *data)
{
	int k;
	void *newpointer;
	size_t prevcount;

	prevcount=(*pvec)->count;
	(*pvec)->count=count;
	newpointer=realloc(*pvec, sizeof(CVHead)+cv_bytes(*pvec));
	if(!newpointer)
	{
		printf(
			"\n\nRUNTIME ERROR\n"
			"realloc returned 0 on vector assign\n"
			"original pointer = %p, original count = %d, assign count = %d\n\n",
			*pvec, prevcount, count);
		scanf("%d", &k);
		abort();
	}
	*pvec=(CVec)newpointer;
	if(data)
		memcpy((*pvec)->data, data, cv_bytes(*pvec));
	else
		memset((*pvec)->data, 0, cv_bytes(*pvec));
}
#define	cv_assign(CVEC, COUNT, DATA)			cv_assign_v(&(CVEC), COUNT, DATA)
#define	cv_assign_zero(CVEC, COUNT)				cv_assign_v(&(CVEC), COUNT, 0)

#define	cv_begin(CVEC)				(CVEC)->data
#define	cv_end(CVEC)				((CVEC)->data+cv_bytes(CVEC))
#define	cv_advance(CVEC, ITER)		(ITER+=(CVEC)->esize)
#define	cv_get(TYPE, ITER)			(TYPE)(ITER)

#define	cv_patv(CVEC, IDX)			((CVEC)->data+cv_offset(CVEC, IDX))
#define	cv_pat(TYPE, CVEC, IDX)		(TYPE*)cv_patv(CVEC, IDX)
#define	cv_at(TYPE, CVEC, IDX)		*cv_pat(TYPE, CVEC, IDX)
#define	cv_back(TYPE, CVEC)			cv_at(TYPE, CVEC, (CVEC)->count-1)
static void* cv_cpat_v(CVec vec, size_t idx)//checked-pointer-at, returns void pointer
{
	cv_check_idx(vec, idx, 0);
	return vec->data+cv_offset(vec, idx);
}
#define	cv_cpat(TYPE, CVEC, IDX)	(TYPE*)cv_cpat_v(CVEC, IDX)
#define	cv_cat(TYPE, CVEC, IDX)		*cv_cpat(TYPE, CVEC, IDX)

static void* cv_insert_v(CVec *pvec, size_t idx, size_t count, const void *data)//X  modify the vec arg
{
	void *newpointer;
	size_t prevcount;
	cv_check_idx(*pvec, idx, 1);

	prevcount=(*pvec)->count;
	(*pvec)->count+=count;
	cv_realloc(pvec,
		"\n\nRUNTIME ERROR\n"
		"realloc returned 0 on vector insert\n"
		"original pointer = %p, original count = %d, insert index = %d, insert count = %d\n\n",
		*pvec, prevcount, idx, count);
	newpointer=cv_patv(*pvec, idx);
	memmove(cv_patv(*pvec, idx+count), newpointer, cv_offset(*pvec, prevcount-idx));
	if(data)
		memcpy(newpointer, data, cv_offset(*pvec, count));
	else
		memset(newpointer, 0, cv_offset(*pvec, count));
	return newpointer;
}
static void* cv_erase_v(CVec *pvec, size_t idx, size_t count)
{
	cv_check_idx(*pvec, idx, 0);
	cv_check_idx(*pvec, idx+count-1, 0);

	memmove(cv_patv(*pvec, idx), cv_patv(*pvec, idx+count), cv_offset(*pvec, (*pvec)->count-(idx+count)));
	(*pvec)->count-=count;
	cv_realloc(pvec,
		"\n\nRUNTIME ERROR\n"
		"realloc returned 0 on vector erase\n"
		"original pointer = %p, original count = %d, erase index = %d, erase count = %d\n\n",
		*pvec, (*pvec)->count+count, idx, count);
	return cv_patv(*pvec, idx);
}
static void* cv_replace_v(CVec *pvec, size_t idx, size_t rem_count, size_t ins_count, const void *data)
{
	ptrdiff_t diff;
	size_t prevcount;
	cv_check_idx(*pvec, idx, 0);
	cv_check_idx(*pvec, idx+rem_count-1, 0);

	diff=ins_count-rem_count;
	prevcount=(*pvec)->count;
	(*pvec)->count+=diff;
	if(diff>0)//will grow
	{
		cv_realloc(pvec,
			"\n\nRUNTIME ERROR\n"
			"realloc returned 0 on vector replace\n"
			"original pointer = %p, original count = %d, index = %d, erase count = %d, insert count = %d\n\n",
			*pvec, prevcount, idx, rem_count, ins_count);
		memmove(cv_patv(*pvec, idx+ins_count), cv_patv(*pvec, idx+rem_count), cv_offset(*pvec, prevcount-(idx+rem_count)));
	}
	else//will shrink
	{
		memmove(cv_patv(*pvec, idx+ins_count), cv_patv(*pvec, idx+rem_count), cv_offset(*pvec, prevcount-(idx+rem_count)));
		cv_realloc(pvec,
			"\n\nRUNTIME ERROR\n"
			"realloc returned 0 on vector replace\n"
			"original pointer = %p, original count = %d, index = %d, erase count = %d, insert count = %d\n\n",
			*pvec, prevcount, idx, rem_count, ins_count);
	}
	if(data)
		memcpy(cv_patv(*pvec, idx), data, cv_offset(*pvec, ins_count));
	else
		memset(cv_patv(*pvec, idx), 0, cv_offset(*pvec, ins_count));
	return cv_patv(*pvec, idx);
}
#define	cv_insert1(CVEC, IDX, ELEM)				cv_insert_v(&(CVEC), IDX, 1, &(ELEM))
#define	cv_insert(CVEC, IDX, COUNT, DATA)		cv_insert_v(&(CVEC), IDX, COUNT, DATA)
#define	cv_insert_zero(CVEC, IDX, COUNT)		cv_insert_v(&(CVEC), IDX, COUNT, 0)
#define	cv_erase1(CVEC, IDX)					cv_erase_v(&(CVEC), IDX, 1)
#define	cv_erase(CVEC, IDX, COUNT)				cv_erase_v(&(CVEC), IDX, COUNT)
#define	cv_replace(CVEC, IDX, CREM, CINS, DATA)	cv_replace_v(&(CVEC), IDX, CREM, CINS, DATA)

#define	cv_push_back(CVEC, ELEM)				cv_insert_v(&(CVEC), (CVEC)->count, 1, &(ELEM))
#define	cv_pop_back(CVEC, ELEM)					cv_erase_v(&(CVEC), (CVEC)->count-1, 1)

#define	cv_str_push_back(CVEC, ELEM)			cv_insert_v(&(CVEC), (CVEC)->count-1, 1, &(ELEM))
#define	cv_str_pop_back(CVEC, ELEM)				cv_erase_v(&(CVEC), (CVEC)->count-2, 1)
#define	cv_str_append(CVEC, COUNT, DATA)		cv_insert_v(&(CVEC), (CVEC)->count-1, COUNT, DATA)
#define	cv_str_cutoff(CVEC, COUNT)				cv_erase_v(&(CVEC), (CVEC)->count-1-(COUNT), COUNT)

#ifdef __cplusplus
}
#endif

#endif