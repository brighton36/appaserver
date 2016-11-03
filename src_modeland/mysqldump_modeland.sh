:
# ------------------------------------
# src_modeland/mysqldump_modeland.sh
# ------------------------------------
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 parameter_file" 1>&2
	exit 1
fi

parameter_file=$1
database=modeland

mysqldump_database.sh $database "$parameter_file"

return_value=$?

if [ "$return_value" -ne 0 ]
then
	echo "$0 exiting early." 1>&2
	exit 1
fi

exit 0

