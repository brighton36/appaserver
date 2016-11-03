:
# utility/cpio2tape_etc.sh
# -----------------------------------
#
# Tim Riley
# ------------------------------------

uname=`uname -n`

if [ "$uname" = "appaserver" ]
then
	output_device=/dev/nht0
	cpio_h_flag=newc
elif [ "$uname" = "snook" ]
then
	output_device=/dev/nht0
	cpio_h_flag=odc
else
	output_device=/dev/nht0
	cpio_h_flag=newc
fi

if [ "$#" -ne 1 -o "$1" != "y" ]
then
	echo "Usage: $0 really_yn" 1>&2

	echo 1>&2
	echo "This script uses the command: cpio -ov -H $cpio_h_flag" 1>&2
	echo "to backup /etc." 1>&2
	echo "It compresses and then sends them" 1>&2
	echo "to ${output_device}." 1>&2
	echo 1>&2
else
	cd /etc
	find . -print  					|
	cpio -ov -H $cpio_h_flag			|
	compress > $output_device
fi

results=$?

# For some reason a success returns 2 also.
# -----------------------------------------
if [ $results -ne 0 -a $results -ne 2 ]
then
	echo "Error number $results occurred. Backup not complete!" 1>&2
	exit $results
else
	echo "To restore:" 1>&2
	echo "1) cd /etc" 1>&2
	echo "2) zcat < $output_device | cpio -ivdu" 1>&2
	echo "" 1>&2
	echo "To rewind the tape use: mt -f $output_device rewind" 1>&2
	echo "To forward the tape once use: mt -f $output_device fsf 1" 1>&2
	echo "" 1>&2
	exit 0
fi
