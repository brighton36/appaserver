:
# utility/cpio_source.sh
# ----------------------

if [ "$#" -ne 2 ]
then
	echo \
"Usage: $0 destination_directory destination_filename" 1>&2
	exit 1
fi

destination_directory=$1
destination_filename=$destination_directory/$2

today=`date.sh | column.e 0 | sed 's|/|-|g'`
filename_file=/tmp/cpio_source_${today}.txt

if [ `uname -n` = "snook" ]
then
	cpio_h_flag=odc
else
	cpio_h_flag=newc
fi

find . -type f \( -name '*.c' -o 			\
		  -name '*.h' -o 			\
		  -name '*.js' -o 			\
		  -name '*.java' -o 			\
		  -name '*.sh' -o 			\
		  -name '*.pc' -o 			\
		  -name '*.py' -o 			\
		  -name 'trashcan.gif' -o 		\
		  -name 'makefile' \) -follow -print	|
grep -v "/backup/"					|
grep -v "/backup_old/"					|
grep -v "/trash/"					|
grep -v "/tmp/"						|
grep -v "/easycharts/"					|
tee $filename_file					|
cpio -ov -H $cpio_h_flag				|
gzip > $destination_filename

# Send the destination filename to stdout
# ---------------------------------------
echo "$destination_filename"

# Send the filename count to stderr
# ---------------------------------
echo "Filename count = `wc -l $filename_file`" 1>&2

exit 0
