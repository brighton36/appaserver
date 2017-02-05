/* library/post_login_library.h						*/
/* --------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------------	*/

#ifndef POST_LOGIN_LIBRARY_H
#define POST_LOGIN_LIBRARY_H

#define RETURN_ADDRESS			"login@cloudacus.com"
#define SUBJECT				"Appaserver login"

/*
#define EMAIL_OUTPUT_FILE_TEMPLATE	"%s/%s/post_login_%s.html"
#define EMAIL_HTTP_FILE_TEMPLATE	"%s://%s/%s/post_login_%s.html"
*/

enum password_match_return {	password_match,
				password_fail,
				public_login,
				email_login };

boolean post_login_email_login(
					char *login_name,
					char *email_http_filename );

void post_login_redraw_index_screen(	char *application_name,
					char *location,
					char *message );

enum password_match_return post_login_password_match(
					char *application_name,
					char *login_name,
					char *password,
					char *database_password );

void post_login_output_frameset(	char *application_name,
					char *database_string,
					char *login_name,
					char *session,
					enum password_match_return );

boolean post_login_encoded_password_ok(
					char *application_name,
					char *database_password,
					char *password );

char *post_login_get_encoded_password(
					char *application_name,
					char *password,
					boolean old_password );

#endif
