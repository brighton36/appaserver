#!/bin/sh
# ------------------------------------
# get_related_record_list.sh
# ------------------------------------
#
# Copyright 1999-01 by Appahost
# ---------------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo \
	"Usage: $0 entity application folder role relation" 1>&2
	exit 1
fi

entity=$1
application=$2
folder=$3
role=$4
relation=$5

relation_table=`get_table_name $entity "" relation`
application_role=`get_table_name $entity "" application_role`
role_folder=`get_table_name $entity "" role_folder`

if [ "$relation" = "1:m" ]
then
	echo "select 	distinct					\
			$relation_table.folder,				\
			related_attribute, 				\
			folder_set_yn,					\
			soft_relation_yn				\
	      from $relation_table, $application_role, $role_folder	\
	      where $relation_table.related_folder =			\
		    $role_folder.folder					\
		and $role_folder.role = $application_role.role		\
		and $application_role.application = '$application'	\
		and related_folder = '$folder'				\
		and $role_folder.role = '$role'				\
	      order by display_order;"					|
	sql.e								|
	tee_process.e "cat					       |\
		       grep '\^null\^'				       |\
		       piece '^' 0			       	       |\
		       xargs.e get_related_record_list.sh		\
					$entity				\
					$application			\
					{}				\
					'1:m'				"
else
	echo "select 	distinct					\
			related_folder,					\
			related_attribute, 				\
			folder_set_yn,					\
			soft_relation_yn				\
	      from $relation_table, $application_role, $role_folder	\
	      where $relation_table.folder = $role_folder.folder	\
		and $role_folder.role = $application_role.role		\
		and $application_role.application = '$application'	\
		and $relation_table.folder = '$folder'			\
		and $role_folder.role = '$role'				\
	      order by display_order;"					|
	sql.e								|
	cat
fi

