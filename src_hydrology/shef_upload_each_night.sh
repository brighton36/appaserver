:
# shef_upload_each_night.sh
# -------------------------

. /home/kotun/set_path.sh 2>/dev/null

if [ "$#" -ne 4 ]
then
	echo \
"Usage: $0 entity source_directory email_results_address with_file_trim_yn" 1>&2
	exit 1
fi

entity=$1
source_directory=$2
email_address=$3
with_file_trim_yn=$4

execution_directory="/opt/html/src_hydrology"
last_shef_file_date=`cat $source_directory/last_shef_file`

if [ "$?" -ne 0 ]
then
	echo "$0 exiting early" 1>&2
	exit 1
fi

last_shef_file="E${last_shef_file_date}.coe"
echo "Uploading $last_shef_file" 1>&2

cd $execution_directory
shef_upload	$entity 				\
		hydrology 				\
		${source_directory}/${last_shef_file}	\
		$with_file_trim_yn 			\
		$email_address				\
		y
