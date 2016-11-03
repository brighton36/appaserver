:
# --------------------
# utility/mysqldump.sh
# --------------------
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 application each_line_insert_yn" 1>&2
	exit 1
fi

application=$1

if [ "$2" = "y" ]
then
	options="-u root -p -c --skip-extended-insert --force --quick --add-drop-table"
else
	options="-u root -p -c --force --quick --add-drop-table"
fi

outputfile=/tmp/mysqldump_${application}_`date.e 0 | tr ':' '-'`.sql.gz


nice -9 mysqldump $options $application				|
gzip 								|
cat > $outputfile

echo "Created $outputfile"

exit 0
