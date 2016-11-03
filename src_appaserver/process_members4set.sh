#!/bin/sh
# ---------------------------------
# process_members4set.sh
# ---------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 entity process_set role" 1>&2
	exit 1
fi

entity=$1
process_set=$2
role=$3

role_process_set_member=`get_table_name $entity role_process_set_member`
process=`get_table_name $entity process`

echo "select $process.process,$process.process_set_display	\
      from $role_process_set_member,$process			\
      where role = '$role'					\
	and process_set = '$process_set'			\
	and	$role_process_set_member.process =		\
		$process.process				\
      order by $process.process;"				|
sql.e

