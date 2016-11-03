#!/bin/sh
# application_record.sh
# ---------------------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 application [delimiter]" 1>&2
	exit 1
fi

application=$1

if [ "$#" -eq 2 ]
then
	delimiter=$2
else
	delimiter="^"
fi

table_name=${application}_application

echo "	select 	application_title,		\
		only_one_primary_yn,		\
		relative_source_directory,	\
		background_color,		\
		distill_directory,		\
		ghost_script_directory,		\
		grace_home_directory,		\
		grace_execution_directory,	\
		grace_free_option_yn,		\
		grace_output,			\
		frameset_menu_horizontal_yn,	\
		ssl_support_yn,			\
		prepend_http_protocol_yn,	\
		max_query_rows_for_drop_downs,	\
		max_drop_down_size,		\
		chart_email_command_line,	\
		appaserver_version		\
	from $table_name			\
	where application = '$application';"	|
sql.e "$delimiter"

