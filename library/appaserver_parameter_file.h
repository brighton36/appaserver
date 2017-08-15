/* library/appaserver_parameter_file.h					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_PARAMETER_FILE_H
#define APPASERVER_PARAMETER_FILE_H

#include "boolean.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define APPASERVER_PARAMETER_DEFAULT_DIRECTORY		"/etc"
#define APPASERVER_PARAMETER_FILE_NAME			"appaserver.config"
#define APPASERVER_PARAMETER_APPLICATION_FILE_NAME	"appaserver_%s.config"
#define APPASERVER_PARAMETER_FILE_DBMS			"mysql"

/* Type definitions */
/* ---------------- */
typedef struct
{
	char *default_database_connection;
	char *user;
	char *password;
	char *flags;
	char *MYSQL_HOST;
	char *MYSQL_TCP_PORT;
	char *appaserver_mount_point;
	char *apache_cgi_directory;
	char *cgi_home;
	char *document_root;
	char *appaserver_error_directory;
	char *appaserver_data_directory;
	char *parameter_file_full_path;
	boolean mysql_password_syntax;

	/* Trying to retire: */
	/* ----------------- */
	char *database_management_system;

} APPASERVER_PARAMETER_FILE;

/* Prototypes */
/* ---------- */
/*
void appaserver_parameter_file_get_dictionary_format(
						APPASERVER_PARAMETER_FILE *s,
						FILE *f );
*/

APPASERVER_PARAMETER_FILE *new_appaserver_parameter_file(
						void );

char *appaserver_parameter_file_get_appaserver_mount_point(
						void );

char *appaserver_parameter_file_get_cgi_directory(
						void );

char *appaserver_parameter_file_get_apache_cgi_directory(
						void );

char *appaserver_parameter_file_get_appaserver_error_directory(
						void );

char *appaserver_parameter_file_get_appaserver_data_directory(
						void );

char *appaserver_parameter_file_get_database_management_system(
						void );

char *appaserver_parameter_file_get_dbms(
						void );

char *appaserver_parameter_file_get_database(
						void );

char *appaserver_parameter_file_get_cgi_home(
						void );

char *appaserver_parameter_file_get_document_root(
						void );

DICTIONARY *appaserver_parameter_file_load_record_dictionary(
						FILE *input_pipe,
						int delimiter );

APPASERVER_PARAMETER_FILE *appaserver_parameter_file_new(
						void );

APPASERVER_PARAMETER_FILE *appaserver_parameter_file_fetch(
					FILE *f,
					char *parameter_file_full_path );

APPASERVER_PARAMETER_FILE *appaserver_parameter_default_file_new(
						void );

APPASERVER_PARAMETER_FILE *appaserver_parameter_file_application(
						char *application_name );

#endif
