#!/bin/sh
# ---------------------------------------------
# src_appaserver/folder_load.sh
# and
# library/folder_load.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

# echo "Starting: $0 $*" 1>&2

if [ "$#" -lt 1 ]
then
	echo \
	"Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_folder=`get_table_name $application folder`

row_access_count="'0'"

echo "select 		$table_folder.folder,				\
			insert_rows_number,				\
			lookup_email_output_yn,				\
			populate_drop_down_process,			\
			post_change_process,				\
			form,						\
			notepad,					\
			html_help_file_anchor,				\
			post_change_javascript,				\
			$row_access_count,				\
			lookup_before_drop_down_yn,			\
			no_initial_capital_yn,				\
			subschema,					\
			data_directory,					\
			index_directory					\
      		from	$table_folder;"					|
sql.e

exit 0
