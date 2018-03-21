/* $APPASERVER_HOME/library/array.h			   */
/* ------------------------------------------------------- */
/* This is a wrapper around arrays.			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef ARRAY_H
#define ARRAY_H

typedef struct
{
	void **base;
	size_t length;
} ARRAY;

ARRAY *array_new(	size_t length );

int array_set(		void **base,
			size_t length,
			void *item,
			int position );

void *array_get(	void **base,
			size_t length,
			int position );

int array_sort_string(	void **base,
			size_t length );

int array_string_compare(
			const void *s1,
			const void *s2 );

#endif
