#!/bin/bash
# -----------------------------------------------------------------
# $APPASERVER_HOME/src_accountancymodel/export_accountancy_model.sh
# -----------------------------------------------------------------
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$(echo $1 | piece.e ':' 0)
database=$(echo $1 | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
else
	export DATABASE=$application
fi

appaserver_config_file="/etc/appaserver.config"

label="appaserver_mount_point="
appaserver_home=`cat $appaserver_config_file	| \
		 grep "^${label}"		| \
		 sed "s/$label//"`

input_file=${appaserver_home}/src_accountancymodel/accountancy_model_folders.dat

parameter_list=`
cat $input_file				|
count.e 1 				|
sed 's/ //'				|
sed 's/)/=/'				|
sed 's/^0//'				|
sed 's/^/folder_/'			|
joinlines.e '&'				|
cat`

export_subschema $application ignored ignored ignored ignored "$parameter_list"

exit 0
