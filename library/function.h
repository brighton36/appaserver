/* /usr2/lib/function.h */
/* -------------------- */

#ifndef FUNCTION_H
#define FUNCTION_H

#define MAX_FUNCTION_ADT	100

typedef struct
{
	struct
	{
		char *name;
		int (*entry)( char *d, char *entire_function );
	} function[ MAX_FUNCTION_ADT + 1 ];
} FUNCTION_ADT;


/* Function prototypes */
/* ------------------- */
FUNCTION_ADT *function_init();
void set_function( FUNCTION_ADT *f, char *name, int (*entry)() );
int function_free();
char *replace_all_functions( char *d, char *s, FUNCTION_ADT *f );
int execute_function_adt( 	char *function_results,
				char *function_name,
				FUNCTION_ADT *f,
				char *entire_function );

#endif
