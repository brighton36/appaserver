/* library/populate_helper_process.h					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef POPULATE_HELPER_PROCESS_H
#define POPULATE_HELPER_PROCESS_H

#include <unistd.h>
#include "boolean.h"
#include "application.h"

/*
#define HELPER_PROCESS_OUTPUT_FILE_TEMPLATE	"%s/%s/helper_process_%s_%d.html"
#define HELPER_PROCESS_HTTP_FTP_FILE_TEMPLATE	"%s://%s/%s/helper_process_%s_%d.html"
#define HELPER_PROCESS_FTP_FILE_TEMPLATE	"/%s/helper_process_%s_%d.html"
*/

typedef struct
{
	char *key;
} POPULATE_HELPER_PROCESS;

POPULATE_HELPER_PROCESS *
	populate_helper_process_new(
				char *key );

char *populate_helper_process_get_output_filename(
				char *key,
				char *document_root_directory,
				char *application_name,
				pid_t process_id );

char *populate_helper_process_get_ftp_file_prompt(
				char *key,
				char *application_name,
				pid_t process_id );

#endif
