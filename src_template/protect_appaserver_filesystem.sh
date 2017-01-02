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

	if [ "`umask`" != "0002" ]
	then
		echo "Error: umask needs to be set to 0002." 1>&2
		echo "Try: sudo sh" 1>&2
		echo "Then: . ${profile_file}" 1>&2
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

protect_cgi_home ()
{
	group=$1
	cgi_home=$2
	execute=$3

	if [ "$execute" = "execute" ]
	then
		chgrp $group ${cgi_home}
		chmod g+rwxs ${cgi_home}
		chmod o-w ${cgi_home}
	else
		echo "chgrp $group ${cgi_home}"
		echo "chmod g+rwxs ${cgi_home}"
		echo "chmod o-w ${cgi_home}"
	fi
}

protect_document_root_template ()
{
	group=$1
	execute=$2

	template_directory="${DOCUMENT_ROOT}/template"

	if [ "$execute" = "execute" ]
	then
		chgrp $group $template_directory
		chmod g+rwxs $template_directory
		chmod o-w $template_directory
		chmod g+w ${template_directory}/index.php
		chmod g+w ${template_directory}/style.css

		chgrp $group ${template_directory}/appaserver
		chmod g+rwxs $template_directory/appaserver
		chmod o-w ${template_directory}/appaserver
		chmod g+w ${template_directory}/appaserver/index.php
		chmod g+w ${template_directory}/appaserver/style.css

		chgrp $group ${template_directory}/images
		chmod g+rwxs $template_directory/images
		chmod o-w ${template_directory}/images
		chmod g+w ${template_directory}/images/logo_appaserver.jpg
	else
		echo "chgrp $group $template_directory"
		echo "chmod g+rwxs $template_directory"
		echo "chmod o-w $template_directory"
		echo "chmod g+w ${template_directory}/index.php"
		echo "chmod g+w ${template_directory}/style.css"

		echo "chgrp $group ${template_directory}/appaserver"
		echo "chmod g+rwxs $template_directory/appaserver"
		echo "chmod o-w ${template_directory}/appaserver"
		echo "chmod g+w ${template_directory}/appaserver/index.php"
		echo "chmod g+w ${template_directory}/appaserver/style.css"

		echo "chgrp $group ${template_directory}/images"
		echo "chmod g+rwxs $template_directory/images"
		echo "chmod o-w ${template_directory}/images"
		echo "chmod g+w ${template_directory}/images/logo_appaserver.jpg"
	fi
}

protect_document_root ()
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

protect_appaserver_error_directory ()
{
	group=$1
	appaserver_error=$2
	execute=$3

	if [ "$execute" = "execute" ]
	then
		chgrp $group $appaserver_error
		chmod g+rwxs $appaserver_error
		chmod o-rwx $appaserver_error
	else
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

protect_old_appaserver_error_file ()
{
	group=$1
	execute=$2

	old_appaserver_error_file="/var/log/appaserver.err"

	if [ "$execute" = "execute" ]
	then
		chgrp $group $old_appaserver_error_file
		chmod g+rw $old_appaserver_error_file
		chmod o-r $old_appaserver_error_file
	else
		echo "chgrp $group $old_appaserver_error_file"
		echo "chmod g+rw $old_appaserver_error_file"
		echo "chmod o-rw $old_appaserver_error_file"
	fi
}

protect_error_file_template ()
{
	group=$1
	appaserver_error=$2
	execute=$3

	template_error_file="${appaserver_error}/appaserver_template.err"

	if [ "$execute" = "execute" ]
	then
		chgrp $group $template_error_file
		chmod g+rw $template_error_file
		chmod o-rw $template_error_file
	else
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

protect_appaserver_data ()
{
	group=$1
	appaserver_data=$2
	execute=$3

	if [ "$execute" = "execute" ]
	then
		chgrp $group $appaserver_data
		chmod g+rwxs $appaserver_data
		chmod o-rwx $appaserver_data
	else
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
protect_cgi_home $group $cgi_home $execute
protect_document_root $group $execute
protect_appaserver_home $group $execute
protect_appaserver_error_directory $group $appaserver_error $execute
protect_old_appaserver_error_file $group $execute
protect_appaserver_data $group $appaserver_data $execute
protect_appaserver_config $group $execute
protect_error_file_template $group $appaserver_error $execute
protect_document_root_template $group $execute

exit 0