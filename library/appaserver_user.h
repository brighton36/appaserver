/* appaserver_user.h 							*/
/* -------------------------------------------------------------------- */
/* This is the appaserver user ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef APPASERVER_USER_H
#define APPASERVER_USER_H

#include "boolean.h"
#include "list.h"

#define APPASERVER_USER_RECORD_DELIMITER	'^'

typedef struct
{
	char *login_name;
	char *person_full_name;
	char *password;
	LIST *role_list;
	char frameset_menu_horizontal_yn;
	LIST *session_list;
} APPASERVER_USER;

/* Prototypes */
/* ---------- */
LIST *appaserver_user_get_session_list(	char *application_name,
					char *login_name );

boolean appaserver_user_exists_session(	char *application_name,
					char *login_name,
					char *session );

boolean appaserver_user_exists_role(	char *application_name,
					char *login_name,
					char *role_name );

APPASERVER_USER *appaserver_user_new_appaserver_user(
					char *application_name,
					char *login_name );

char *appaserver_user_get_person_full_name(
					char *application_name,
					char *login_name );
char *appaserver_user_get_password(	char *application_name,
					char *login_name );

char appaserver_user_get_frameset_menu_horizontal_yn(
					char *application_name,
					char *login_name );

LIST *appaserver_user_get_role_list(
					char *application_name,
					char *login_name );
#endif
