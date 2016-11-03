:
# runme.sh
# --------------------
# This loads CR10 data
# --------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 all|date_offset_comma_list" 1>&2
	exit 1
fi

date_offset_comma_list=$1

if [ "$date_offset_comma_list" = "all" ]
then
	date_offset_comma_list=date_offset_comma_list
fi

host_name=`uname -n`
application=everglades

if [ "$host_name" = "snook" ]
then
	execute_directory=/opt/html/cgi
	data_directory=/opt/export/enpdata
else
	execute_directory=`get_appaserver_mount_point`/src_$application
	data_directory=`get_appaserver_mount_point`/$application
fi

cd $execute_directory

./load_cr10	$application 				\
		$data_directory 			\
		'[A-Z]*.DAT' 				\
		station 				\
		$date_offset_comma_list 		\
		y 					\
		email_address 				\
		y 2>&1					|
sed "s/<p>//g"						|
grep -v "Duplicate"

