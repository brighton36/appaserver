:
# src_hydrology/archive_cr10_file_done.sh
# ---------------------------------------

if [ "$#" -ne 3 ]
then
	echo "Usage: $0 application station file2process" 1>&2
	exit 1
fi

application=$1
station=$2
file2process=$3

done_directory=`get_cr10_done_directory.sh $application`
destination_filename=$done_directory/${station}.DAT

if [ ! -f $destination_filename ]
then
	> $destination_filename 1>&2
fi

if [ ! -w $destination_filename ]
then
	echo "ERROR in $0: cannot write to $destination_filename" 1>&2
	exit 1
fi

echo "<br>Archiving file $file2process to $destination_filename"
cat $file2process >> $destination_filename

# put a <CR> at the end because it is missing
# -------------------------------------------
echo "" >> $destination_filename

# Set to full permissions because of write errors.
# ------------------------------------------------
chmod go+w $destination_filename 2>/dev/null

exit 0
