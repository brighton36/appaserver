:
# migrate_destination_measurement_backup.sh
# --------------------------------------

# Run this after mysqldump_database.sh finishes.
# ----------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 date" 1>&2
	exit 1
fi

date=$1

# Expect to take 2 hours.
# -----------------------
cd /dfe/son
zcat measurement_backup_$date.sql.gz | sql.e hydrology

exit 0
