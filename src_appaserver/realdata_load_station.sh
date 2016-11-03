:
# realdata_load_station.sh
# ------------------------

echo "Starting: $0 $*" 1>&2

if [ "$#" -ne 5 ]
then
	echo \
"Usage: $0 entity filename station datatype really_yn" 1>&2
	exit 1
fi

entity=$1
filename=$2
station=$3
datatype=$4
really_yn=$5

cat $filename						      	      	      |
grep '^[0-9]'								      |
prepend_century_maybe						      	      |
column2pipe.e 3								      |
sed 's/$/|null/'							      |
piece_swap.e '|' 3 4							      |
sed 's/|/-/1'								      |
sed 's/|/-/1'								      |
sed "s/^/$datatype|/"							      |
sed "s/^/$station|/"							      |
tr '|' ','								      |
measurement_insert $entity realdata $really_yn		      		      |
cat

if [ "$really_yn" = "y" ]
then
	echo "	update measurement			\
		set measurement_value = null		\
		where station = '$station'		\
		  and datatype = '$datatype'		\
		  and measurement_value < -999999;"	|
	sql.e
fi

exit 0

