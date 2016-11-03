:
# read_tee.sh
# ------------
# -- Tim Riley
# ------------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 filename [append]" 1>&2
	exit 1
fi

filename=$1

if [ "$#" -eq 2 -a "$2" = "append" ]
then
	append=yes
else
	append=no
fi

if [ "$append" = no ]
then
	> $filename
fi

while read input
do
	echo $input >> $filename
	echo $input
done
