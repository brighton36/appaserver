:
if [ "$#" -ne 2 ]
then
	echo "Usage: $0 directory process" 1>&2
	exit 1
fi

directory=$1
process=$2

inotifywait -m -e close_write $directory 2>/dev/null	|
column.e 2						|
while read newfile
do
	$process $newfile
done

exit 0
