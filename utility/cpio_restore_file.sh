#!/bin/sh
#--------
# cpio_restore_file.sh
# --------------------
# ---------
# Tim Riley
# ---------

if [ "$#" -eq 0 ]
then
	echo \
"Usage: cpio_restore_file.sh archive_file.cpio.Z [dir/file]"
	exit 1
fi


zcat < $1 | cpio -ivd $2
if [ "$?" -ne 0 ]
then
	echo "$0 exiting early"
	exit 1
fi

echo "Done"
exit 0
