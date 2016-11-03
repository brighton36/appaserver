:
# ----------------------
# operations4folder.sh
# ----------------------
#
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

# echo "Starting $0 $*" 1>&2

if [ "$#" -ne 3 ]
then
	echo \
	"Usage: $0 entity folder role" 1>&2
	exit 1
fi

entity=$1
folder=$2
role=$3

role_operation_table=`get_table_name $entity role_operation`
#folder_operation_table=`get_table_name $entity folder_operation`
operation_table=`get_table_name $entity operation`

#echo "select $operation_table.operation					\
#      from $operation_table,						\
#           $role_operation_table,					\
#           $folder_operation_table					\
#      where $folder_operation_table.folder = '$folder'			\
#        and $role_operation_table.role = '$role'			\
#        and $operation_table.operation =				\
#            $folder_operation_table.operation				\
#        and $folder_operation_table.operation =				\
#            $role_operation_table.operation				\
#        and $folder_operation_table.folder =				\
#            $role_operation_table.folder;"				|
#sql.e '|'								|
#sort -u

echo "select $operation_table.operation					\
      from $operation_table,						\
           $role_operation_table					\
      where 1 = 1							\
        and $role_operation_table.role = '$role'			\
        and $operation_table.operation =				\
            $role_operation_table.operation				\
        and $role_operation_table.folder = '$folder';"			|
sql.e '|'								|
sort -u

exit 0
