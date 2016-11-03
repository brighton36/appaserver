/* frameset.h 								*/
/* -------------------------------------------------------------------- */
/* This is the appaserver frameset ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef FRAMESET_H
#define FRAMESET_H

#include "list.h"
#include "appaserver_library.h"

/* Constants */
/* --------- */
#define TOP_FRAME_NAME			"top_frame"
#define HEADING_FRAME_NAME		"heading_frame"

/* Structures */
/* ---------- */
typedef struct
{
	char *frameset_name;
	char *form_name;
	char *login_name;
	char *session;
	char *application_name;
	char *folder_or_process;
	char *role;
	char *state;
	char *error_file;
	char *blank_form_name;
	char *prompt_destination_filename;
	char *create_destination_filename;
	char *heading_filename;
	char *prompt_bottom_filename;
	char *create_bottom_filename;
	int output_bottom_frame;
	char *dictionary_string;
	char *target_frame;
	char *sys_string;
	char *appaserver_mount_point;
} FRAMESET;

/* Operations */
/* ---------- */
FRAMESET *frameset_new( char *frameset_name, char *form_name );

void frameset_set_parameters( 	FRAMESET *frameset,
				char *login_name, 
				char *application_name,
				char *session, 
				char *folder_or_process, 
				char *role,
				char *state, 
				char *appaserver_mount_point, 
				char *error_file );

void frameset_set_appaserver_mount_point(
				FRAMESET *frameset,
				char *appaserver_mount_point );
void frameset_set_dictionary_string(
				FRAMESET *frameset,
				char *dictionary_string );
void frameset_set_output_bottom_frame(
				FRAMESET *frameset );
void frameset_remove_files( 	FRAMESET *frameset );
char *frameset_get_prompt_destination_filename(
				FRAMESET *frameset );
char *frameset_get_create_destination_filename(
				FRAMESET *frameset );
char *frameset_get_create_bottom_filename(
				FRAMESET *frameset );
char *frameset_get_prompt_bottom_filename(
				FRAMESET *frameset );
void frameset_output_by_cat( 	FRAMESET *frameset );
void frameset_output_by_frameset(
				FRAMESET *frameset );
void frameset_set_target_frame( FRAMESET *frameset, 
				char *target_frame );
void frameset_set_sys_string(	FRAMESET *frameset,
				char *sys_string );
void frameset_set_application_name(
				FRAMESET *frameset,
				char *application_name );
void frameset_set_customer(	FRAMESET *frameset,
				char *customer );
void frameset_set_session(	FRAMESET *frameset,
				char *session );
char *frameset_get_heading_filename(
				char *application_name,
				char *frameset_name,
				char *session );
void frameset_set_appaserver_mount_point(
				FRAMESET *frameset,
				char *appaserver_mount_point );
#endif

/* void frameset_output(	 	FRAMESET *frameset ); */
