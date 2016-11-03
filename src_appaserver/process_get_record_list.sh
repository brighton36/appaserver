#!/bin/sh
# ---------------------------------------------
# src_appaserver/process_get_record_list.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

# echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 1 ]
then
	echo \
"Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application process`

echo "select	process,						\
		command_line,						\
		notepad,						\
		html_help_file_anchor,					\
		post_change_javascript,					\
		process_set_display,					\
		preprompt_help_text,					\
		appaserver_yn						\
      from $table_name;"						|
sql.e

