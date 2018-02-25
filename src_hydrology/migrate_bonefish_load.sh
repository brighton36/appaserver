:
# migrate_bonefish_load.sh
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
#nohup time migrate_bonefish_tables.sh $date >/dfe/tmp/hydrology_tables.out 2>&1 &

# Expect to take 7 1/2 hours.
# ---------------------------
#nohup time migrate_bonefish_measurement.sh $date >/dfe/tmp/hydrology_measurement.out 2>&1 &

# Expect to take 2 hours.
# -----------------------
#nohup time migrate_bonefish_measurement_backup.sh $date >/dfe/tmp/hydrology_measurement.out 2>&1 &

cd /dfe/son/sparrow
rm *
nohup time
(
tar xzf ../sparrow_$date.tar.gz && ls -1 *.sql |
while read tablefile
do
	echo "Loading $tablefile"
	cat $tablefile | sql.e hydrology
done
) >/dfe/tmp/sparrow.out 2>&1 &

exit 0
