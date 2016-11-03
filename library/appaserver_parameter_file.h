/* libraryh/appaserver_parameter_file.h					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_PARAMETER_FILE_H
#define APPASERVER_PARAMETER_FILE_H

#include "boolean.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define APPASERVER_PARAMETER_FILE_NAME		"appaserver.config"

/* These are being retired. */
/* ------------------------ */
#define NODE_PIECE				0
#define PASSWORD_PIECE				1
#define USER_PIECE				2
#define FLAGS_PIECE				3
#define DATABASE_PIECE				4
#define MYSQL_HOST_PIECE			5
#define MYSQL_TCP_PORT_PIECE			6
#define MYSQL_PWD_PIECE				7
#define APPASERVER_MOUNT_POINT_PIECE		8
#define CGI_DIRECTORY_PIECE			9
#define DBMS_PIECE			       10
#define ORACLE_UIDPWD_PIECE		       11
#define APPASERVER_PARAMETER_FILE_DELIMITER	'|'

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
	char *MYSQL_PWD;
	char *appaserver_mount_point;
	char *apache_cgi_directory;
	char *database_management_system;
	char *dynarch_home;
	char *cgi_home;
	char *document_root;
	char *appaserver_error_directory;
	char *appaserver_data_directory;
	char *parameter_file_full_path;
	boolean mysql_password_syntax;
} APPASERVER_PARAMETER_FILE;

/* Prototypes */
/* ---------- */
void appaserver_parameter_file_get_dictionary_format(
				APPASERVER_PARAMETER_FILE *s,
				FILE *f );

void appaserver_parameter_file_parse_single_line(
				APPASERVER_PARAMETER_FILE *s,
				char *buffer );

APPASERVER_PARAMETER_FILE *new_appaserver_parameter_file( void );
void appaserver_parameter_file_get_piece(	char *d, 
						char *s, 
						int p, 
						char *filename );

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

char *appaserver_parameter_file_get_dynarch_home(
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

#endif
