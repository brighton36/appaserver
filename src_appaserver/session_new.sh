#!/bin/sh
# ----------------------------------------------
# $APPASERVER_HOME/src_appaserver/session_new.sh
# ----------------------------------------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

echo Starting: $0 $* 1>&2

if [ $# -ne 2 ]
then
	echo "Usage: $0 application login_name" 1>&2
	exit 1
fi

application=$1
login_name=$2

now_date=`date.e 0 | piece.e ':' 0`
appaserver_sessions_table=`get_table_name $application appaserver_sessions`
now_time=`date.e 0 | piece.e ':' 1`
session_number=`session_number_new.sh $application`
http_user_agent=`echo "$HTTP_USER_AGENT" | trim_length.e 80`

echo "	insert into $appaserver_sessions_table				 \
	(appaserver_session,login_name,http_user_agent,			 \
	 remote_ip_address,login_date,login_time,			 \
	 last_access_date,last_access_time)				 \
	values								 \
	('$session_number','$login_name','$http_user_agent',		 \
	 '$REMOTE_ADDR','$now_date','$now_time',			 \
	 '$now_date','$now_time');"			 		 |
sql.e

echo $session_number

exit 0

