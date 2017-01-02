:
# Freely available software. See appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 execute|noexecute" 1>&2
	exit 1
fi

execute=$1

profile_file="/etc/profile"
appaserver_config_file="/etc/appaserver.config"

integrity_check ()
{
	results=`	cat $appaserver_config_file		|\
			grep mysql_password=			|\
			grep secret`

	if [ "$results" != "" ]
	then
		echo "You need to set the mysql password." 1>&2
		echo "Check $appaserver_config_file" 1>&2
		exit 1
	fi

	if [ "$USER" = "root" ]
	then
		echo "You cannot be root to run this." 1>&2
		echo "Dont't: sudo $0" 1>&2
		exit 1
	fi

	if [ "$DOCUMENT_ROOT" = "" ]
	then
		echo "DOCUMENT_ROOT must be set." 1>&2
		echo "Try: . $profile_file" 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "APPASERVER_HOME must be set." 1>&2
		echo "Try: . $profile_file" 1>&2
		exit 1
	fi

}

create_template_database ()
{
	echo "create database template;" | sql.e
}

load_mysqldump_template ()
{
	cd /tmp
	rm mysqldump_template.sql 2>/dev/null
	wget timriley.net/download/appaserver/mysqldump_template.sql
	cat mysqldump_template.sql | sql.e '^' mysql template
}

integrity_check
create_template_database
load_mysqldump_template

upgrade_appaserver_database template:template

exit 0
