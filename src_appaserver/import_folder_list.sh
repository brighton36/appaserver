:
if [ "$#" -ne 1 ]
then
	echo "Usage: $0 source_directory" 1>&2
	exit 1
fi

ls $1/*.dat | xargs.e basename {} | sed 's/\..*$//'
