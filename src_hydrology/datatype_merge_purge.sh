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

if [ "$#" -ne 3 ]
then
	exit_usage=1
fi

if [	"$2" != "countgood" -a		\
	"$2" != "countbad" -a		\
	"$2" != "update" -a		\
	"$2" != "delete" ]
then
	exit_usage=1
fi

infile=$1
operation=$2
head_only_yn=$3

if [ "$exit_usage" -eq 1 ]
then
	echo "Usage: $0 infile countgood|countbad|update|delete head_only_yn" 1>&2
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

if [ "$head_only_yn" = "y" ]
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
	bad_datatype=`echo $record | column.e 1`
	good_datatype=`echo $record | column.e 2`
	begin_date=`echo $record | column.e 3`
	end_date=`echo $record | column.e 4`

	if [ "$operation" = "countgood" ]
	then
		generate_where $station $good_datatype $begin_date $end_date

		/bin/echo -n "$station/${good_datatype}: "
		echo "select count(*) from measurement where $where;" |
		sql.e
	fi

	if [ "$operation" = "countbad" ]
	then
		generate_where $station $bad_datatype $begin_date $end_date

		/bin/echo -n "$station/${bad_datatype}: "
		echo "select count(*) from measurement where $where;" |
		sql.e
	fi

	if [ "$operation" = "update" ]
	then
		generate_where $station $bad_datatype $begin_date $end_date

		echo "update measurement set datatype = '$good_datatype' where $where;"
	fi

	if [ "$operation" = "delete" ]
	then
		generate_where $station $good_datatype $begin_date $end_date

		echo "delete from measurement where $where;"
	fi

done

exit 0

