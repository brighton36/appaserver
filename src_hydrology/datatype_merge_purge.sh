:
# $APPASERVER_HOME/src_hydrology/datatype_merge_purge.sh
# ------------------------------------------------------
# Freely available software. See appaserver.org
# ------------------------------------------------------

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
	"$2" != "head" -a		\
	"$2" != "full" ]
then
	exit_usage=1
fi

infile=$1
operation=$2

if [ "$exit_usage" -eq 1 ]
then
	echo "Usage: $0 infile count|head|full" 1>&2
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

if [ "$operation" = "head" ]
then
	head_process="head -10"
else
	head_process="cat"
fi

cat $infile				|
$head_process				|
while read record
do
	station=`echo $record | column.e 0`
	purge_datatype=`echo $record | column.e 1`
	keep_datatype=`echo $record | column.e 2`
	begin_date=`echo $record | column.e 3`
	end_date=`echo $record | column.e 4`

	if [ "$operation" = "count" ]
	then
		echo "Count keep:"
		echo "-----------"
		generate_where $station $keep_datatype $begin_date $end_date

		/bin/echo -n "$station/${keep_datatype}: "
		echo "select count(*) from measurement where $where;" |
		sql.e

		echo "Count purge:"
		echo "------------"
		generate_where $station $purge_datatype $begin_date $end_date

		/bin/echo -n "$station/${purge_datatype}: "
		echo "select count(*) from measurement where $where;" |
		sql.e
	fi

	if [ "$operation" = "full" -o "$operation" = "head" ]
	then
		generate_where $station $keep_datatype $begin_date $end_date

		echo "delete from measurement where $where;"

		generate_where $station $purge_datatype $begin_date $end_date

		echo "update measurement set datatype = '$keep_datatype' where $where;"
	fi

done

exit 0

