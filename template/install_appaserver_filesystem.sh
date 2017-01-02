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
	group=$1
	cgi_home=$2

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
		echo "Try: . ${profile_file}" 1>&2
		exit 1
	fi

	if [ "$APPASERVER_HOME" = "" ]
	then
		echo "Error: APPASERVER_HOME must be set." 1>&2
		echo "Try: . ${profile_file}" 1>&2
		exit 1
	fi

	if [ "`umask`" != "0002" ]
	then
		echo "Error: umask needs to be set to 0002." 1>&2
		echo "Try: . ${profile_file}" 1>&2
		exit 1
	fi

	if [ "$SUDO_USER" = "" ]
	then
		echo "Error: Cannot determine your regular user name." 1>&2
		echo "Try: sudo sh" 1>&2
		echo "Then: . ${profile_file}" 1>&2
		exit 1
	fi

	group_exists=`grep "^${group}" /etc/group`

	if [ "$group_exists" = "" ]
	then
		echo "Error: Group $group doesn't exist in /etc/group" 1>&2
		exit 1
	fi

	user_exists=`grep "^${group}" /etc/group | grep $SUDO_USER`

	if [ "$user_exists" = "" ]
	then
		echo "Error: User $SUDO_USER doesn't belong to group $group in /etc/group" 1>&2
		exit 1
	fi
}

change_permissions_cgi_home ()
{
	group=$1
	cgi_home=$2
	execute=$3

	if [ "$execute" = "execute" ]
	then
		chgrp $group ${cgi_home}
		chmod g+rwxs ${cgi_home}
		chmod o-rwx ${cgi_home}
	else
		echo "chgrp $group ${cgi_home}"
		echo "chmod g+rwxs ${cgi_home}"
		echo "chmod o-rwx ${cgi_home}"
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

change_permissions_document_root ()
{
	group=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		chgrp $group $DOCUMENT_ROOT
		chmod g+rwxs $DOCUMENT_ROOT
		chmod o-rwx $DOCUMENT_ROOT
	else
		echo "chgrp $group $DOCUMENT_ROOT"
		echo "chmod g+rwxs $DOCUMENT_ROOT"
		echo "chmod o-rwx $DOCUMENT_ROOT"
	fi
}

create_appaserver_error_directory ()
{
	group=$1
	appaserver_error=$2
	execute=$3

	if [ "$execute" = "execute" ]
	then
		mkdir $appaserver_error
		chgrp $group $appaserver_error
		chmod g+rwxs $appaserver_error
		chmod o-rwx $appaserver_error
	else
		echo "mkdir $appaserver_error"
		echo "chgrp $group $appaserver_error"
		echo "chmod g+rwxs $appaserver_error"
		echo "chmod o-rwx $appaserver_error"
	fi
}

protect_appaserver_home ()
{
	group=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		chgrp $group $APPASERVER_HOME
		chmod g+rwxs $APPASERVER_HOME
		chmod o-w $APPASERVER_HOME
	else
		echo "chgrp $group $APPASERVER_HOME"
		echo "chmod g+rwxs $APPASERVER_HOME"
		echo "chmod o-w $APPASERVER_HOME"
	fi
}

create_old_appaserver_error_file ()
{
	group=$1
	execute=$2

	old_appaserver_error_file="/var/log/appaserver.err"

	if [ "$execute" = "execute" ]
	then
		touch $old_appaserver_error_file
		chgrp $group $old_appaserver_error_file
		chmod g+rw $old_appaserver_error_file
		chmod o-r $old_appaserver_error_file
	else
		echo "touch $old_appaserver_error_file"
		echo "chgrp $group $old_appaserver_error_file"
		echo "chmod g+rw $old_appaserver_error_file"
		echo "chmod o-rw $old_appaserver_error_file"
	fi
}

create_error_file_template ()
{
	group=$1
	appaserver_error=$2
	execute=$3

	template_error_file="${appaserver_error}/appaserver_template.err"

	if [ "$execute" = "execute" ]
	then
		touch $template_error_file
		chgrp $group $template_error_file
		chmod g+rw $template_error_file
		chmod o-rw $template_error_file
	else
		echo "mkdir $template_error_file"
		echo "chgrp $group $template_error_file"
		echo "chmod g+rw $template_error_file"
		echo "chmod o-rw $template_error_file"
	fi
}

protect_appaserver_config ()
{
	group=$1
	execute=$2

	if [ "$execute" = "execute" ]
	then
		chgrp $group $appaserver_config_file
		chmod g+rw $appaserver_config_file
		chmod o-rw $appaserver_config_file
	else
		echo "chgrp $group $appaserver_config_file"
		echo "chmod g+rw $appaserver_config_file"
		echo "chmod o-rw $appaserver_config_file"
	fi
}

create_appaserver_data ()
{
	group=$1
	appaserver_data=$2
	execute=$3

	if [ "$execute" = "execute" ]
	then
		mkdir $appaserver_data
		chgrp $group $appaserver_data
		chmod g+rwxs $appaserver_data
		chmod o-rwx $appaserver_data
	else
		echo "mkdir $appaserver_data"
		echo "chgrp $group $appaserver_data"
		echo "chmod g+rwxs $appaserver_data"
		echo "chmod o-rwx $appaserver_data"
	fi
}

label="appaserver_group="
group=`	cat $appaserver_config_file	|\
	grep "^${label}"		|\
	sed "s/$label//"`

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

integrity_check $group $cgi_home
change_permissions_cgi_home $group $cgi_home $execute
change_permissions_document_root $group $execute
protect_appaserver_home $group $execute
create_appaserver_error_directory $group $appaserver_error $execute
create_old_appaserver_error_file $group $execute
create_appaserver_data $group $appaserver_data $execute
protect_appaserver_config $group $execute
create_error_file_template $group $appaserver_error $execute
create_document_root_template $execute
copy_document_root_template $execute
link_document_root $execute

exit 0
