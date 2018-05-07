:
# migrate_destination_load.sh
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
nohup time migrate_destination_tables.sh $date >/dfe/tmp/hydrology_tables.out 2>&1 &

# Expect to take 7 1/2 hours.
# ---------------------------
nohup time migrate_destination_measurement.sh $date >/dfe/tmp/hydrology_measurement.out 2>&1 &

# Expect to take 2 hours.
# -----------------------
nohup time migrate_destination_measurement_backup.sh $date >/dfe/tmp/hydrology_measurement_backup.out 2>&1 &

nohup time migrate_destination_sparrow.sh $date >/dfe/tmp/sparrow.out 2>&1 &

exit 0
