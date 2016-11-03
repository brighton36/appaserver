:
# shef_list.sh
# ------------
#
# -- Tim Riley
# ------------
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 entity" 1>&2
	exit 1
fi

entity=$1

table_name=`get_table_name $entity shef_datatype`

echo "	select station, datatype, shef_upload_code, shef_download_code	\
	from $table_name;"						|
sql.e '|'								|
toupper.e
