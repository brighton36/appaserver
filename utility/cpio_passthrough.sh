:
# cpio_passthrough.sh
# -------------------

if [ "$#" -ne 2 ]
then
	(
	echo "Usage: 	cpio_passthrough.sh source_dir destination_dir"
	echo "Example: 	cpio_passthrough.sh /usr/local/source /tmp/destination"
	echo
	echo "This script uses cpio to copy a directory tree"
	echo "someplace else."
	) 1>&2
	exit 1
fi

destination=$2

if [ "$destination" = "." ]
then
	destination=`pwd`
fi

results=`strncmp.e "$destination" '.' 1`
if [ "$results" -eq 0 ]
then
	echo "Error: destination cannot contain a dot." 1>&2
	exit 1
fi

cd "$1" && find . -print | cpio -pvd --unconditional "$destination"

exit 0
