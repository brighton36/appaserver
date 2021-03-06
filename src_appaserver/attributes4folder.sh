#!/bin/sh
# ----------------------------------------------------
# $APPASERVER_HOME/src_appaserver/attributes4folder.sh
# ----------------------------------------------------

if [ "$#" -lt 2 ]
then
	echo \
"Usage: $0 application folder [primary]" 1>&2
	echo \
"Note:  without the 3rd parameter, \"all\" is assumed." 1>&2
	exit 1
fi

application=$1
folder=$2

if [ "$#" -eq 3 -a "$3" = "primary" ]
then
	primary_key_clause=" and primary_key_index >= 1"
else
	primary_key_clause=""
fi

folder_attribute=`get_table_name $application folder_attribute`

echo "select $folder_attribute.attribute			\
      from $folder_attribute					\
      where $folder_attribute.folder = '$folder'		\
      $primary_key_clause					\
      order by display_order,primary_key_index;"		|
sql.e

exit 0
