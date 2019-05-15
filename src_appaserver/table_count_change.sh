:
# $APPASERVER_HOME/src_appaserver/table_count_change.sh
# -----------------------------------------------------

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

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 table" 1>&2
	exit 1
fi

table=$1

tmp_directory="/var/tmp"
last_time_file="$tmp_directory/`basename.e $0 y`_$table.dat"

if [ -r $last_time_file ]
then
	last_time_count=`cat $last_time_file`
else
	last_time_count=0
fi

current_count=`	echo "select count(*) from $table;"	|\
		sql.e					|\
		tee $last_time_file`

difference=`expr $current_count - $last_time_count`

echo "Current:    $current_count"
echo "Last:       $last_time_count"
echo "Difference: $difference"

exit 0

