:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 uid/pwd" 1>&2
	exit 1
fi

uidpwd=$1

rm measurement.fifo 2>/dev/null
mkfifo measurement.fifo

echo "Executing sqlload"
gunzip -c measurement.txt.gz > measurement.fifo &
sqlload userid=$uidpwd control=measurement.ctl

return_value=$?

rm measurement.fifo

if [ "$return_value" -ne 0 ]
then
	echo "$0 exiting early"
	exit 1
fi

echo "Creating indexes"
measurement_oracle_create_indexes.sh $uidpwd

echo $0 complete

exit 0
