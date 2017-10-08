:
# $APPASERVER_HOME/utility/adduser.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 username [group]" 1>&2
	exit 1
fi

username=$1

if [ "$#" -eq 2 ]
then
	group=$2
fi

useradd.sh $username $group

exit 0

