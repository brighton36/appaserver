:
# $APPASERVER_HOME/src_hydrology/station_datatype_merge_purge.sh
# --------------------------------------------------------------
# Freely available software. See appaserver.org
# --------------------------------------------------------------

if [ "$APPASERVER_DATABASE" != "" ]
then
	application=$APPASERVER_DATABASE
elif [ "$DATABASE" != "" ]
then
	application=$DATABASE
fi

if [ "$application" = "" ]
then
	echo "Error in `basename.e $0 n`: you must first:" 1>&2
	echo "\$ . set_database" 1>&2
	exit 1
fi

#
# TPGW-10D salinity salinity_ground_water 2010-09-02 2019-03-20
# ^	   ^	    ^			  ^	     ^
# |	   |	    |			  |	     |
# Station  |	    |			  |	     |
#	   Purge    |			  |	     |
#		    Keep datatype	  |	     |
#					  Begin	     |
#						     End measurement date
# -----------------------------------------------------------------------

exit_usage=0

if [ "$#" -ne 2 ]
then
	exit_usage=1
fi

if [	"$2" != "count" -a		\
	"$2" != "trim" -a		\
	"$2" != "full" -a		\
	"$2" != "execute" ]
then
	exit_usage=1
fi

infile=$1
operation=$2

if [ "$exit_usage" -eq 1 ]
then
	echo "Usage: $0 infile count|trim|full|execute" 1>&2
	exit 1
fi

function generate_where ()
{
	station=$1
	datatype=$2
	begin_date=$3
	end_date=$4

	where="station = '$station' and datatype = '$datatype' and measurement_date between '$begin_date' and '$end_date'"

}
# generate_where()

if [ "$operation" = "trim" ]
then
	trim_process="queue_top_bottom_lines.e 5"
else
	trim_process="cat"
fi

cat $infile				|
grep -v '^#'				|
while read record
do
	station=`echo $record | column.e 0`
	purge_datatype=`echo $record | column.e 1`
	keep_datatype=`echo $record | column.e 2`
	begin_date=`echo $record | column.e 3`
	end_date=`echo $record | column.e 4`

	if [ "$operation" = "count" ]
	then
		generate_where $station $keep_datatype $begin_date $end_date

		/bin/echo -n "Count keep $station/${keep_datatype}: "
		echo "select count(*) from measurement where $where;" |
		sql.e

		generate_where $station $purge_datatype $begin_date $end_date

		/bin/echo -n "Count purge $station/${purge_datatype}: "
		echo "select count(*) from measurement where $where;" |
		sql.e
	fi

	if [ "$operation" = "full" -o	\
	     "$operation" = "trim" -o	\
	     "$operation" = "execute" ]
	then
		if [ "$operation" = "execute" ]
		then
			execute="sql.e"
		else
			execute="cat"
		fi

		generate_where $station $purge_datatype $begin_date $end_date

		f=`folder_attribute_name_list.sh measurement`

		echo "select $f from measurement where $where;"		|
		sql.e '^'						|
		sed "s/${purge_datatype}/${keep_datatype}/"		|
		insert_statement.e table=measurement field=$f del='^'	|
		$trim_process						|
		$execute 2>&1						|
		grep -vi duplicate					|
		cat

		echo "delete from measurement where $where;"		|
		$execute						|
		cat

	fi

done

exit 0

