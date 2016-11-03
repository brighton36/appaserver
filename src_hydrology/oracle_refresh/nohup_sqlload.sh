:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 uid/pwd" 1>&2
	exit 1
fi

uidpwd=$1

nohup sqlload.sh $uidpwd > sqlload.out 2>&1 &

exit $?
