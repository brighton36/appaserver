#!/bin/bash
# -----------------
# changeitcheck.sh
# -----------------
#
# Freely available software: see Appaserver.org
# ---------------------------------------------

function changeitcheck()
{
	application=$1
	login_name=$2

	table=`get_table_name $application application`
	where="application = '$application'"
	
	encrypted_changeit=`echo "select password( 'changeit' ) from $table where $where;" | sql.e`
	
	table=`get_table_name $application appaserver_user`
	where="login_name = '$login_name'"
	
	login_password=`echo "select password from $table where $where;" | sql.e`
	
	if [ "$login_password" = "" ]
	then
		echo "No user or no password"
		exit 0
	fi
	
	if [ "$encrypted_changeit" = "$login_password" ]
	then
		echo "${login_name}: Not Changed"
	else
		echo "${login_name}: Changed"
	fi
}

function changeitcheck_all()
{
	application=$1

	table=`get_table_name $application appaserver_user`
	where="password is not null";

	process='echo "select login_name from $table where $where;" | sql.e'

	eval $process						|
	while read login_name
	do
		changeitcheck $application $login_name
	done
}

if [ $# -ne 1 ]
then
	echo "Usage: $0 application" 1>&2
	exit 1
fi

application=$1

database=$(echo $application | piece.e ':' 1 2>/dev/null)

if [ "$database" != "" ]
then
	export DATABASE=$database
	application=$(echo $application | piece.e ':' 0)
else
	export DATABASE=$application
fi

changeitcheck_all $application

exit 0

