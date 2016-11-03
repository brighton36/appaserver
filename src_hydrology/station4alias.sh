:
# station4alias.sh
# ----------------
if [ "$#" -lt 1 ]
then
	echo "Usage: $0 entity [station_alias]" 1>&2
	exit 1
fi

entity=$1

if [ "$#" -eq 2 ]
then
	station_alias=$2
fi

table_name=`get_table_name $entity station_alias`
PATH=$PATH:.:/opt/html/utility; export PATH

function station4alias
{
	station=`							\
		echo "	select station					\
			from station_alias				\
			where station_alias = '$station_alias';"	|
		sql.e							|
		toupper.e`
	echo $station
}

if [ "$station_alias" != "" ]
then
	
	echo_no_cr.sh "$station_alias|"
	station4alias
else
	while read station_alias
	do
		echo_no_cr.sh "$station_alias|"
		station4alias
	done
fi
