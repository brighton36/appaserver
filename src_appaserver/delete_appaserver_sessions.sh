:
# src_appaserver/delete_appaserver_sessions.sh
# --------------------------------------------

if [ "$#" -eq 0 ]
then
	echo \
"Usage: $0 application[:database] [application...]" 1>&2
	exit 1
fi

PATH=$PATH:.; export PATH

while [ $# -gt 0 ]
do
	application=$1

	database=$(echo $1 | piece.e ':' 1 2>/dev/null)

	if [ "$database" != "" ]
	then
		export DATABASE=$database
	fi

	table_name=`get_table_name $application appaserver_sessions`
	echo "delete from $table_name;" | sql.e
	shift
done

exit 0
