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
	cgi_home=$1
	execute=$2

	if [ "$execute" = "execute" -a "$USER" != "root" ]
	then
		echo "Error: You must be root to run this." 1>&2
		echo "Try: sudo $0" 1>&2
		exit 1
	fi

	if [ ! -d "${cgi_home}" ]
	then
		echo "Error: ${cgi_home} is not found in $appaserver_config_file" 1>&2
		exit 1
	fi

	if [ "$DOCUMENT_ROOT" = "" ]
	then
		echo "Error: DOCUMENT_ROOT must be set." 1>&2
		echo "Try: sudo $0" 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "Error: APPASERVER_HOME must be set." 1>&2
		echo "Try: sudo $0" 1>&2
		exit 1
	fi
}

remove_document_root_appaserver ()
{
	execute=$1

	appaserver_directory="${DOCUMENT_ROOT}/appaserver"

	if [ "$execute" = "execute" ]
	then
		rm -fr $appaserver_directory
	else
		echo "rm -fr $appaserver_directory"
	fi
}

remove_appaserver_error_directory ()
{
	appaserver_error=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		rm -fr $appaserver_error
	else
		echo "rm -fr $appaserver_error"
	fi
}

remove_old_appaserver_error_file ()
{
	execute=$1

	old_appaserver_error_file="/var/log/appaserver.err"

	if [ "$execute" = "execute" ]
	then
		rm $old_appaserver_error_file
	else
		echo "rm $old_appaserver_error_file"
	fi
}

remove_appaserver_data ()
{
	appaserver_data=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		rm -fr $appaserver_data
	else
		echo "rm -fr $appaserver_data"
	fi
}

enable_apache_cgi ()
{
	if [ "$execute" = "execute" ]
	then
		a2enmod cgi
		apache2ctl restart
	else
		echo "a2enmod cgi"
		echo "apache2ctl restart"
	fi
}

label="cgi_home="
cgi_home=`	cat $appaserver_config_file	|\
	 	grep "^${label}"		|\
	 	sed "s/$label//"`

label="appaserver_error_directory="
appaserver_error=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

label="appaserver_data_directory="
appaserver_data=`	cat $appaserver_config_file	|\
	 		grep "^${label}"		|\
	 		sed "s/$label//"`

. $profile_file

integrity_check $cgi_home $execute
remove_appaserver_error_directory $appaserver_error $execute
remove_old_appaserver_error_file $execute
remove_appaserver_data $appaserver_data $execute
remove_document_root_appaserver $execute

echo ""
echo "You may also need to:"
echo '1) echo "drop database template;" | sql.e'
echo '2) cd $APPASERVER_HOME && make clean'
echo '3) rm -fr $APPASERVER_HOME'
echo "4) rm /etc/appaserver.config"
echo "5) trim /etc/profile of appaserver variables"

exit 0
