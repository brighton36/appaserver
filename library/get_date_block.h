/* get_date_block.h */
/* ---------------- */

#ifndef GET_DATE_BLOCK_H
#define GET_DATE_BLOCK_H

#define DATE_GROUP_DAILY	0
#define DATE_GROUP_WEEKLY	1
#define DATE_GROUP_MONTHLY	2
#define DATE_GROUP_ANNUALLY	3

int get_date_block(	char **block,
			char delimiter,
			int date_offset,
			int date_group,
			FILE *input_file );
char *date_group4date(	char *date_field, 
			int date_group );
char *date_group_this_saturday( 
			char *date_field );
char *date_group_end_of_month( 
			char *date_field );
char *date_group_end_of_year( 
			char *date_field );
#endif
