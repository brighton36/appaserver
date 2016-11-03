:
# Freely available software. See appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 group" 1>&2
	exit 1
fi

group=$1

integrity_check ()
{
	if [ "$USER" != "root" ]
	then
		echo "You must be root to run this." 1>&2
		echo "Try: sudo sh" 1>&2
		echo "Then:. /etc/profile" 1>&2
		exit 1
	fi

	if [ ! -d /usr/lib/cgi-bin ]
	then
		echo "Mysql doesn't seem to be installed." 1>&2
		echo "Try: sudo apt-get -y install mysql-server" 1>&2
		exit 1
	fi

	if [ "$DOCUMENT_ROOT" = "" ]
	then
		echo "DOCUMENT_ROOT must be set." 1>&2
		echo "Try: . /etc/profile" 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "APPASERVER_HOME must be set." 1>&2
		echo "Try: . /etc/profile" 1>&2
		exit 1
	fi

}

append_etc_profile ()
{
	echo "" >> /etc/profile
	echo "# Added by $0" >> /etc/profile
	echo "# ======================================" >> /etc/profile
	echo "umask 0002" >> /etc/profile
	echo "export APPASERVER_HOME=/var/appaserver" >> /etc/profile
	echo "export CGI_HOME=/usr/lib/cgi-bin" >> /etc/profile
	echo "export APPASERVER_UNUSED_PARAMETER_FLAG=-Wunused-parameter" >> /etc/profile
	echo "export DOCUMENT_ROOT=/var/www" >> /etc/profile
	echo 'export PATH=$PATH:$APPASERVER_HOME/utility:$APPASERVER_HOME/src_appaserver:.' >> /etc/profile
	echo "#export APPASERVER_LINK_FLAGS=-s" >> /etc/profile
	echo "set -o vi" >> /etc/profile
}

change_permissions_cgi_bin ()
{
	group=$1
	cd /usr/lib
	chgrp $group cgi-bin
	chmod g+rwxs cgi-bin
	chmod o-rx cgi-bin
}

change_permissions_var_www ()
{
	group=$1
	cd $DOCUMENT_ROOT
	chgrp $group .
	chmod g+rwxs .
	chmod o-rwx .
}

create_appaserver_error_directory ()
{
	group=$1
	cd /var/log
	mkdir appaserver
	chgrp $group appaserver
	chmod g+rwxs appaserver
	chmod o-rwx appaserver
}

create_var_appaserver ()
{
	group=$1
	cd /var
	mkdir appaserver
	chgrp $group appaserver
	chmod g+rwxs appaserver
	chmod o-w appaserver
}

create_old_appaserver_error_file ()
{
	cd /var/log/appaserver
	/bin/echo > appaserver.err
	chmod o-r appaserver.err
}

create_template_error_file ()
{
	cd /var/log/appaserver
	/bin/echo > appaserver_template.err
	chmod o-r appaserver_template.err
}

create_appaserver_config ()
{
	group=$1
	cd /etc
	cp $APPASERVER_HOME/appaserver_template.config appaserver.config
	chgrp $group appaserver.config
	chmod g+w appaserver.config
	chmod o-r appaserver.config
}

create_var_appaserver_data ()
{
	cd $APPASERVER_HOME
	mkdir data
	chmod o-rx data
}

integrity_check
append_etc_profile
change_permissions_cgi_bin $group
change_permissions_var_www $group
create_var_appaserver $group
create_appaserver_error_directory $group
create_old_appaserver_error_file
create_template_error_file
create_appaserver_config $group
create_var_appaserver_data

exit 0
