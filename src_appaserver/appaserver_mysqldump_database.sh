:
# ----------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_mysqldump_database.sh
# ----------------------------------------------------------------

appaserver_config_file="/etc/appaserver.config"
backup_directory="/var/backups/appaserver"

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

ls -1 $backup_directory/mysqldump_*.dat	|
while read parameter_file
do
	if [ -r "$parameter_file" ]
	then
		echo "mysqldump_database.sh $parameter_file"
		mysqldump_database.sh $parameter_file 2>&1
	fi
done

exit 0
