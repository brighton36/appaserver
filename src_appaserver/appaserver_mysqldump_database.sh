:
# ----------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_mysqldump_database.sh
# ----------------------------------------------------------------

appaserver_config_file="/etc/appaserver.config"
backup_directory="/var/backups/appaserver"

# Get directory of error files
# ----------------------------
label="appaserver_error_directory="
appaserver_error=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

if [ ! -d "${appaserver_error}" ]
then
	echo "Error: ${appaserver_error} is not found in $appaserver_config_file" 1>&2
	exit 1
fi

# Get appaserver_home
# -------------------
label="appaserver_mount_point="
appaserver_home=`cat $appaserver_config_file	| \
		 grep "^${label}"		| \
		 sed "s/$label//"`

if [ ! -d "${appaserver_home}" ]
then
	echo "Error: ${appaserver_home} is not found in $appaserver_config_file" 1>&2
	exit 1
fi

export APPASERVER_HOME=$appaserver_home
export PATH=$PATH:$APPASERVER_HOME/utility:$APPASERVER_HOME/src_appaserver

# For each database that has mysqldump_{DATABASE}.dat
# ---------------------------------------------------
ls $appaserver_error/*				 |
xargs.e "/usr/bin/basename {}			|\
	 sed 's/appaserver_//'			|\
	 piece.e '.' 0"				 |
while read database
do
	export DATABASE=$database

	parameter_file=${backup_directory}/mysqldump_${DATABASE}.dat

	if [ -r "$parameter_file" ]
	then
		echo "mysqldump_database.sh $DATABASE $parameter_file"
		mysqldump_database.sh $DATABASE $parameter_file 2>&1
	fi
done

exit 0
