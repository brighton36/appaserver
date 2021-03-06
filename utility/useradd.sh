:
# $APPASERVER_HOME/utility/useradd.sh
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

# Create the username and default group
# -------------------------------------
sudo useradd -m $username
sudo usermod -a -G $username $username
sudo usermod $username -s /bin/bash

# Assign to additional group
# --------------------------
if [ "$group" != "" ]
then
	sudo addgroup $group
	sudo usermod -a -G $group $username
fi

sudo passwd $username

exit 0

