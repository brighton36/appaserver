:
# column4table.sh
#
# Tim Riley
# ----------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 schema table_name" 1>&2
	exit 1
fi

file=column4table.dat
directory=`directory4schema.sh $1`
table_name=`tolower.e $2`

cat $directory/$file			|
grep "^$table_name:" 			|
trim_until_character.e ':' 		|
tr ',' '\012'				|
cat
