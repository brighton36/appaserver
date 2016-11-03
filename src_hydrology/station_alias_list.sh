:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

entity=$1

table_name=`get_table_name $entity station_alias`

echo "select station, station_alias from $table_name;"		|
sql.e '|'							|
toupper.e
