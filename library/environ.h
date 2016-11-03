/* library/environ.h					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef ENVIRON_H
#define ENVIRON_H

#include "boolean.h"

char *resolve_environment_variables( 	char *return_string, char *s );
char *environ_get_environment(		char *variable_name );
void set_environment( 			char *environment, char *datum );
void environ_set_environment( 		char *environment, char *datum );
void set_path( 				char *path_to_add );
void add_path( 				char *path_to_add );
boolean environ_variable_exists(	char *variable_name,
					char *looking_string );
boolean environ_name_to_value(		char *variable_value,
					char *variable_name );

int environ_next_delimiter(		char *looking_string );
boolean environ_get_browser_internet_explorer(
					void );
char *environ_get_http_referer(		void );
char *environ_get_http_referer_filename(void );
#endif
