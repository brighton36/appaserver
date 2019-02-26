:
# shef_upload_list.sh
# -------------------
#
# ------------
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

table_name=`get_table_name $application shef_upload_datatype`

echo "	select station, datatype, shef_upload_code			\
	from $table_name;"						|
sql.e '|'								|
#toupper.e								|
cat
