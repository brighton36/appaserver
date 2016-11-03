:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 folder" 1>&2
	exit 1
fi

relation_table=tim_relation
application=general_ledger
folder=$1

echo "select folder, related_attribute			\
      from $relation_table				\
      where application = '$application'		\
	and related_folder = '$folder';"		|
sql.e
