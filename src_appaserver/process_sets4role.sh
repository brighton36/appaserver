#!/bin/sh
# ---------------------------------
# process_sets4role.sh
# ---------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 entity role delimiter" 1>&2
	exit 1
fi

entity=$1
role=$2
delimiter=$3

process_set=`get_table_name $entity process_set`
role_process_set_member=`get_table_name $entity role_process_set_member`

echo "select $role_process_set_member.process_set,			\
	     process_group						\
      from $role_process_set_member,$process_set			\
      where role = '$role'						\
        and $role_process_set_member.process_set =			\
		$process_set.process_set;"				|
sql.e $delimiter							|
sort -u

exit 0

