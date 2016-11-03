#!/bin/sh
# -----------------------------------
# process_records4role.sh
# -----------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

# echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
"Usage: $0 entity role delimiter" 1>&2
	exit 1
fi

entity=$1
role=$2
delimiter=$3

process=`get_table_name $entity process`
role_process=`get_table_name $entity role_process`

echo "select $process.process,						\
	     process_group						\
      from $process, $role_process					\
      where $role_process.role = '$role'				\
	and $process.process = $role_process.process;"			|
sql.e "$delimiter"							|
sort -u

exit 0

