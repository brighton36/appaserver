:
# $APPASERVER_HOME/src_appaserver/table_change_email.sh
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

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 table email" 1>&2
	exit 1
fi

table=$1
email="$2"

change=`table_count_change.sh $table | grep "Change:" | piece.e ':' 1`

message="$table count change: $change"

echo "$message" | mailx -s "$message" "$email"

exit 0
