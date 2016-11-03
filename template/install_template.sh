:
# Freely available software. See appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 version" 1>&2
	exit 1
fi

version=$1

integrity_check ()
{
	results=`	cat /etc/appaserver.config		|\
			grep mysql_password=			|\
			grep secret`
	if [ "$results" != "" ]
	then
		echo "You need to set the mysql password." 1>&2
		echo "Check /etc/appaserver.config" 1>&2
		exit 1
	fi

	if [ "$USER" = "root" ]
	then
		echo "You cannot be root to run this." 1>&2
		echo "Dont't: sudo $0" 1>&2
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

compile_appaserver ()
{
	cd $APPASERVER_HOME
	make_appaserver.sh

	return_value=$?

	if [ $return_value -ne 0 ]
	then
		echo "$0 exiting early." 1>&2
		exit 1
	fi
}

create_template_database ()
{
	echo "create database template;" | sql
}

load_mysqldump_template ()
{
	cd /tmp
	rm mysqldump_template.sql 2>/dev/null
	wget timriley.net/download/appaserver/mysqldump_template.sql
	cat mysqldump_template.sql | sql '^' mysql template
}

link_var_www ()
{
	ln -s $APPASERVER_HOME/template $DOCUMENT_ROOT
	ln -s $APPASERVER_HOME/src_template $DOCUMENT_ROOT
	ln -s $APPASERVER_HOME/javascript $DOCUMENT_ROOT
	ln -s $APPASERVER_HOME/hmenu $DOCUMENT_ROOT
}

download_tar_file ()
{
	version=$1
	cd $APPASERVER_HOME
	rm appaserver_${version}.tar.gz 2>/dev/null
	wget timriley.net/download/appaserver/appaserver_${version}.tar.gz
}

untar_tar_file ()
{
	version=$1
	cd $APPASERVER_HOME
	tar xvzf appaserver_${version}.tar.gz
}

link_icons ()
{
	cd $APPASERVER_HOME
	ln -s `pwd`/trashcan.gif $DOCUMENT_ROOT
	ln -s `pwd`/magnify_glass.gif $DOCUMENT_ROOT
	ln -s `pwd`/top.png $DOCUMENT_ROOT
}

integrity_check
download_tar_file $version
untar_tar_file $version
compile_appaserver
create_template_database
load_mysqldump_template
link_var_www
link_icons

upgrade-appaserver-database template:template

exit 0
