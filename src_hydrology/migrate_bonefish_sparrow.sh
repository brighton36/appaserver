:
# migrate_bonefish_sparrow.sh
# ---------------------------

# Run this after mysqldump_database.sh finishes.
# ----------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 date" 1>&2
	exit 1
fi

date=$1

cd /dfe/son/sparrow
rm *
tar xzf ../sparrow_$date.tar.gz && ls -1 *.sql |
while read tablefile
do
	echo "Loading $tablefile"
	cat $tablefile | sql.e sparrow
done

exit 0
