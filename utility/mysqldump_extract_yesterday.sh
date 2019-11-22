# Must . this file
# -------------------------------------------------------
# $APPASERVER_HOME/utility/mysqldump_extract_yesterday.sh
# -------------------------------------------------------
# Freely available software: see Appaserver.org
# -------------------------------------------------------

if [ $# -lt 1 ]
then
	echo "Usage: . mysqldump_extract_yesterday.sh database [days_ago] [backup_directory]" 1>&2
	echo "Note: days_ago defaults to -1" 1>&2
	echo "Note: backup_directory defaults to /var/backups" 1>&2
else
	database=$1

	if [ $# -gt 1 ]
	then
		days_ago=$2
	else
		days_ago=-1
	fi

	if [ $# -gt 2 ]
	then
		backup_directory=$3
	else
		backup_directory=/var/backups
	fi

	cd $backup_directory

	if [ $? -ne 0 ]
	then
		echo "$0 Warning: cannot change to $backup_directory" 1>&2
	fi

	mysqldump_config=appaserver/mysqldump_${database}.config

	cd $(mysqldump_extract.sh	$database		\
					$mysqldump_config	\
					`now.sh ymd $days_ago`)
fi

# -----------------
# No exit statement
# -----------------
