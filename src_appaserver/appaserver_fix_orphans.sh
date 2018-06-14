:
# ----------------------------------------------------------------
# $APPASERVER_HOME/src_appaserver/appaserver_mysqldump_database.sh
# ----------------------------------------------------------------

appaserver_config_file="/etc/appaserver.config"

# Get appaserver_error_directory
# ------------------------------
label="appaserver_error_directory="
appaserver_error_directory=`	cat $appaserver_config_file	| \
		 		grep "^${label}"		| \
		 		sed "s/$label//"`

if [ ! -d "${appaserver_error_directory}" ]
then
	echo "Error: ${appaserver_error_directory} is not found in $appaserver_config_file" 1>&2
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

ls -1 ${appaserver_error_directory}/appaserver_*.err	|
while read error_file
do
	database=`basename.e $error_file y | piece.e '_' 1`
	export DATABASE=$database
	echo "fix_orphans.sh $database 0 n"
	fix_orphans.sh ignored 0 n 2>&1
done

exit 0
