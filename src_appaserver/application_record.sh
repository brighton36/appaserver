#!/bin/sh
# application_record.sh
# ---------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in $0: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 ignored [delimiter]" 1>&2
	exit 1
fi

if [ "$#" -eq 2 ]
then
	delimiter=$2
else
	delimiter="^"
fi

table_name=${application}_application

# only_one_primary_yn is retired.

echo "	select 	application_title,		\
		'y',				\
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

