:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 entity application" 1>&2
	exit 1
fi

entity=$1
application=$2

table_name=`get_table_name $entity $application shef_alias`

echo "select shef_code, shef_alias from $table_name;"		|
sql.e '|'							|
toupper.e
