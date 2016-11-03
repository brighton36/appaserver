:
# search_file.sh
# ------------------------------------------------------------------------------
# This script searchs a datafile with two columns for a key in the
# first column. It outputs the corresponding entry located in the second column.
# ------------------------------------------------------------------------------
# -- Tim Riley
# ------------------------------------------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 file2search key" 1>&2
	exit 1
fi

file2search=$1
key=$2

cat $file2search			|
grep "^$key[ 	]"			|
column.e 1
