:
# alter_every_mysql_table
# ------------------------------------
# Use this to alter every mysql table.
# ------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 statement_file" 1>&2
	exit 1
fi

statement_file=$1

for table in `echo "show tables;" | mysql.sh`
do
	cat $statement_file			|
	sed "s/\$table/$table/g"		|
	mysql.sh
done
