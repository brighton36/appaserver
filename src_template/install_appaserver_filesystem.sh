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

	if [ "$USER" != "root" ]
	then
		echo "Error: You must be root to run this." 1>&2
		echo "Try: sudo sh" 1>&2
		echo "Then: . ${profile_file}" 1>&2
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
		echo "Try: sudo sh" 1>&2
		echo "Then: . ${profile_file}" 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "Error: APPASERVER_HOME must be set." 1>&2
		echo "Try: sudo sh" 1>&2
		echo "Then: . ${profile_file}" 1>&2
		exit 1
	fi
}

create_document_root_template ()
{
	execute=$1

	template_directory="${DOCUMENT_ROOT}/template"

	if [ "$execute" = "execute" ]
	then
		mkdir $template_directory
		mkdir ${template_directory}/appaserver
		mkdir ${template_directory}/images
	else
		echo "mkdir $template_directory"
		echo "mkdir ${template_directory}/appaserver"
		echo "mkdir ${template_directory}/images"
	fi
}

copy_document_root_template ()
{
	execute=$1

	template_directory="${DOCUMENT_ROOT}/template"
	logo_filename="images/logo_appaserver.jpg"

	if [ "$execute" = "execute" ]
	then
		cp index.php $template_directory
		ln ${template_directory}/index.php ${template_directory}/appaserver/index.php
		cp style.css $template_directory
		ln ${template_directory}/style.css ${template_directory}/appaserver/style.css
		cp $logo_filename ${template_directory}/images
	else
		echo "cp index.php $template_directory"
		echo "ln ${template_directory}/index.php ${template_directory}/appaserver/index.php"
		echo "cp style.css $template_directory"
		echo "ln ${template_directory}/style.css ${template_directory}/appaserver/style.css"
		echo "cp $logo_filename ${template_directory}/images"
	fi
}

link_document_root ()
{
	execute=$1

	src_template_directory="${APPASERVER_HOME}/src_template"
	images_directory="${APPASERVER_HOME}/zimages"
	menu_directory="${APPASERVER_HOME}/zmenu"
	calendar_directory="${APPASERVER_HOME}/zscal2"
	javascript_directory="${APPASERVER_HOME}/javascript"

	if [ "$execute" = "execute" ]
	then
		ln -s $src_template_directory $DOCUMENT_ROOT
		ln -s $images_directory $DOCUMENT_ROOT
		ln -s $menu_directory $DOCUMENT_ROOT
		ln -s $calendar_directory $DOCUMENT_ROOT
		ln -s $javascript_directory $DOCUMENT_ROOT
	else
		echo "ln -s $src_template_directory $DOCUMENT_ROOT"
		echo "ln -s $images_directory $DOCUMENT_ROOT"
		echo "ln -s $menu_directory $DOCUMENT_ROOT"
		echo "ln -s $calendar_directory $DOCUMENT_ROOT"
		echo "ln -s $javascript_directory $DOCUMENT_ROOT"
	fi
}

create_appaserver_error_directory ()
{
	appaserver_error=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		mkdir $appaserver_error
	else
		echo "mkdir $appaserver_error"
	fi
}

create_old_appaserver_error_file ()
{
	execute=$1

	old_appaserver_error_file="/var/log/appaserver.err"

	if [ "$execute" = "execute" ]
	then
		touch $old_appaserver_error_file
	else
		echo "touch $old_appaserver_error_file"
	fi
}

create_error_file_template ()
{
	appaserver_error=$1
	execute=$2

	template_error_file="${appaserver_error}/appaserver_template.err"

	if [ "$execute" = "execute" ]
	then
		touch $template_error_file
	else
		echo "mkdir $template_error_file"
	fi
}

create_appaserver_data ()
{
	appaserver_data=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		mkdir $appaserver_data
	else
		echo "mkdir $appaserver_data"
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

integrity_check $cgi_home
create_appaserver_error_directory $appaserver_error $execute
create_old_appaserver_error_file $execute
create_appaserver_data $appaserver_data $execute
create_error_file_template $appaserver_error $execute
create_document_root_template $execute
copy_document_root_template $execute
link_document_root $execute

exit 0
