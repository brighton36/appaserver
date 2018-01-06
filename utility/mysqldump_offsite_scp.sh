:
# --------------------------------
# utility/mysqldump_offsite_scp.sh
# --------------------------------
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 parameter_file yyyy_mm_dd" 1>&2
	exit 1
fi

parameter_file=$1
date_yyyy_mm_dd=$2
date_no_dashes=`echo $date_yyyy_mm_dd | sed 's/-//g'`

parse_parameter_file()
{
	file=$1
	key=$2

	results=`cat $file | grep "^${key}=" | piece.e "=" 1`
}

error_exit_if_blank()
{
	results=$1
	pattern=$2

	if [ "$results" = "" ]
	then
		echo "Error: cannot find $pattern in parameter_file." 1>&2
		exit 1
	fi
}

offsite_scp()
{
	source_directory=$1
	destination_directory=$2
	date_no_dashes=$3

	logfile="/tmp/offsite_scp_$database.log"

	pwd=`pwd`
	cd $source_directory

	for file in `	ls -1 2>&1 | grep -vi 'no such file'	|\
			grep $date_no_dashes`
	do
		nohup							\
		/usr/bin/time						\
		scp 	${file}						\
			${destination_directory}			\
		>> $logfile 2>&1 &
	done

	cd $pwd
}

parse_parameter_file $parameter_file directory_root
directory_root=$results

parse_parameter_file $parameter_file output_directory
error_exit_if_blank "$results" output_directory
output_directory=$results

if [ "$directory_root" != "" ]
then
	source_directory=${directory_root}/$output_directory
fi

parse_parameter_file $parameter_file offsite_scp_destination
destination_directory=$results

if [ "$destination_directory" != "" ]
then
	offsite_scp $source_directory $destination_directory $date_no_dashes
fi

exit 0
