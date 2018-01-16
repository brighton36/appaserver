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
cd /dfe/son/hydrology
tar xzf ../hydrology_$date.tar.gz
cat *.sql | time sql hydrology >hydrology.log 2>/dfe/tmp/hydrology_time.out &

# Expect to take 7 1/2 hours.
# ---------------------------
cd /dfe/son
zcat measurement_$date.sql.gz | time sql hydrology >/dfe/tmp/measurement.out 2>&1 &

# Expect to take 2 hours.
# -----------------------
zcat measurement_backup_$date.sql.gz | time sql hydrology >/dfe/tmp/measurement_backup.out 2>&1 &

cd /dfe/son/sparrow
tar xzf ../sparrow_$date.tar.gz
cat *.sql | time sql sparrow >sparrow.log 2>/dfe/tmp/sparrow_time.out &

exit 0
