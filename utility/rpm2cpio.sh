:
# rpm2cpio.sh
# -----------

if [ "$#" -lt 2 ]
then
	echo "Usage: $0 source_directory package_without_extension ..." 1>&2
	exit 1
fi

source_directory=$1

while [ "$#" -gt 1 ]
do
	package=$2
	search_string=${source_directory}/${package}
	rpm_filename=`ls $search_string-[1-9]*.rpm 2>/dev/null`

	if [ "$rpm_filename" = "" ]
	then
		echo "Sorry, could not find package: $package" 1>&2
		shift
		continue
	fi

	echo "Found rpm_filename:	" $rpm_filename

	no_directory_filename=`basename $rpm_filename`
	#echo "using no_directory_filename:	" $no_directory_filename

	stripped_filename=`echo $no_directory_filename | sed 's/\.rpm$//'`
	#echo "Using stripped_filename:	" $stripped_filename

	cpio_filename=${stripped_filename}.cpio
	echo "Creating cpio_filename:	" $cpio_filename

	rpm2cpio $rpm_filename > $cpio_filename

	shift

done


exit 0
