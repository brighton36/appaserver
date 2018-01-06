:
# ----------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_database.sh
# ----------------------------------------------
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 database parameter_file" 1>&2
	exit 1
fi

database=$1
parameter_file=$2

export DATABASE=$database

date_yyyy_mm_dd=`date.e 0 | piece.e ':' 0`
appaserver_config=/etc/appaserver.config
mysqluser=root
mysql_password_file=/tmp/mysql_password_file_$$.dat

echo "$0 starting at `date.e 0`"

parse_parameter_file()
{
	file=$1
	key=$2

	results=`cat $file | grep "^${key}=" | piece.e '=' 1`
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

parse_parameter_file $parameter_file directory_root
directory_root=$results

parse_parameter_file $parameter_file output_directory
error_exit_if_blank "$results" output_directory
output_directory=$results

parse_parameter_file $parameter_file processes_in_parallel
error_exit_if_blank "$results" processes_in_parallel
processes_in_parallel=$results

parse_parameter_file $parameter_file each_line_insert
each_line_insert=$results

parse_parameter_file $parameter_file big_table_list
big_table_list=$results

if [ "$directory_root" != "" ]
then
	output_directory=${directory_root}/$output_directory
fi

echo "[mysqldump]" > $mysql_password_file

chmod go-rw $mysql_password_file

grep '^password=' $appaserver_config >> $mysql_password_file

grep '^mysql_password=' $appaserver_config	|
sed 's/mysql_//'				|
cat >> $mysql_password_file

mysqldump_fork.e	$mysqluser			\
			$database			\
			$mysql_password_file		\
			$output_directory		\
			$processes_in_parallel		\
			"$each_line_insert"		\
			$big_table_list

return_value=$?

rm -f $mysql_password_file

if [ "$return_value" -ne 0 ]
then
	echo "mysqldump_fork.e returned an error." 1>&2
	exit 1
fi

mysqldump_appaserver_log.sh $database $parameter_file $date_yyyy_mm_dd
mysqldump_archive_grandfather_father_son.sh $parameter_file $date_yyyy_mm_dd
mysqldump_offsite_rsync.sh $database $parameter_file
mysqldump_offsite_scp.sh $parameter_file $date_yyyy_mm_dd

echo "$0 ending at   `date.e 0`"

exit 0
