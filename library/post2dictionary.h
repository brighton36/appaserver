/* $APPASERVER_HOME/library/post2dictionary.h				*/
/* -------------------------------------------------------------------- */
/* This is the post2dictionary function.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef POST2DICTIONARY_H
#define POST2DICTIONARY_H

#include <stdio.h>
#include "dictionary.h"

#define POST2DICTIONARY_TEMP_DIRECTORY		"/tmp"
#define POST2DICTIONARY_DELIMITER_REPLACEMENT	"**"

void post2dictionary_process_input_buffer(
					DICTIONARY *dictionary,
					char *input_buffer,
					char *post_key,
					char *appaserver_data_directory,
					char *session,
					FILE *input_stream );

DICTIONARY *post2dictionary(		FILE *input_stream,
					char *appaserver_data_directory,
					char *session );

boolean post2dictionary_spool_file(	char *spool_filename,
					FILE *input_stream,
					char *post_key );

void post2dictionary_fetch(		char *destination,
					FILE *input_stream,
					char *post_key );

void post2dictionary_set_dictionary(	DICTIONARY *dictionary,
					char *key,
					char *data );

void post2dictionary_get_spool_filename(
					char *spool_filename,
					char *appaserver_data_directory,
					char *base_filename,
					char *session );

void convert_folder_data_delimiter(
					char *data,
					char *key );

#endif
