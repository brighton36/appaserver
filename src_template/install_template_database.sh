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
		echo "Don't: sudo $0" 1>&2
		exit 1
	fi

	if [ "$DOCUMENT_ROOT" = "" ]
	then
		echo "DOCUMENT_ROOT must be set." 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "APPASERVER_HOME must be set." 1>&2
		exit 1
	fi

	if [ ! -x ${APPASERVER_HOME}/src_appaserver/sql.e ]
	then
		echo "sql.e doesn't exist. Did you compile?"
		exit 1
	fi
}

create_template_database ()
{
	execute=$1

	if [ "$execute" = "execute" ]
	then
		echo "create database template;" | sql.e mysql
	else
		echo 'echo "create database template;" | sql.e mysql'
	fi
}

update_ssl_support ()
{
	execute=$1

	if [ "$execute" = "execute" ]
	then
		echo "update template_application set ssl_support_yn = 'n';" |
		sql.e '^' mysql template
	else
		echo 'echo "update template_application set ssl_support_yn = n;" | sql.e ^ mysql template'

	fi
}

load_mysqldump_template ()
{
	execute=$1

	if [ "$execute" = "execute" ]
	then
		cd /tmp
		rm mysqldump_template.sql.gz 2>/dev/null
		wget timriley.net/download/appaserver/mysqldump_template.sql.gz
		zcat mysqldump_template.sql.gz | sql.e '^' mysql template
	else
		echo "cd /tmp"
		echo "rm mysqldump_template.sql.gz 2>/dev/null"
		echo "wget timriley.net/download/appaserver/mysqldump_template.sql.gz"
		echo "zcat mysqldump_template.sql.gz | sql.e '^' mysql template"
	fi
}

. $profile_file

integrity_check
create_template_database $execute
load_mysqldump_template $execute
update_ssl_support $execute

exit 0
