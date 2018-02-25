:
# migrate_bonefish_tables.sh
# ------------------------

# Run this after mysqldump_database.sh finishes.
# ----------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 date" 1>&2
	exit 1
fi

date=$1

# Expect to take 7 hours.
# -----------------------
cd /dfe/son/hydrology
rm *
tar xzf ../hydrology_$date.tar.gz && ls -1 *.sql |
while read tablefile
do
	echo "Loading $tablefile"
	cat $tablefile | sql.e hydrology
done

exit 0
