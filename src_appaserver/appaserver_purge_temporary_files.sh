#!/bin/sh
# -----------------------------------
# appaserver_purge_temporary_files.sh
# -----------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

delay_minutes=30

if [ -f ./appaserver.config ]
then
	appaserver_config="./appaserver.config"
else
	appaserver_config="/etc/appaserver.config"
fi

appaserver_mount_point=`cat $appaserver_config			|
			grep '^appaserver_mount_point='		|
			piece.e '=' 1`

find	$appaserver_mount_point					\
	-xdev							\
	-maxdepth 2						\
	-name '*_[1-9][0-9][0-9][0-9]*'				\
	-cmin +$delay_minutes					\
	-exec rm -f {} \; 2>/dev/null

exit 0
