:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

entity=$1

table_name=`get_table_name $entity shef_alias`

echo "select shef_code, shef_alias from $table_name;"		|
sql '|'								|
toupper.e
