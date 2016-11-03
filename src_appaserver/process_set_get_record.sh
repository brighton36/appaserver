#!/bin/sh
# ---------------------------------------------
# process_get_set_record.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
"Usage: $0 application session process_set" 1>&2
	exit 1
fi

application=$1
session=$2
process_set=$3

table_name=`get_table_name $application process_set`

echo "select	notepad,						\
		html_help_file_anchor,					\
		post_change_javascript,					\
		prompt_display_text,					\
		preprompt_help_text,					\
		prompt_display_bottom_yn				\
      from $table_name							\
      where process_set = '$process_set';"				|
sql.e

exit 0

