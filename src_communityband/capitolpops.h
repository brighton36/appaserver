/* src_capitolpops/capitolpops.h					*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef CAPITOLPOPS_H
#define CAPITOLPOPS_H

#define RECORD_LABEL ".record "
#define TOTAL_LABEL ".total "

#define CONTACT_MESSAGE_TEMPLATE		\
"This message was automatically generated from capitolpops.org: I just joined the Capitol Pops substitution list as a %s player."

void post_capitolpops_redraw(	char *redraw_message );

#endif
