:
# $APPASERVER_HOME/utility/userdel.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 username" 1>&2
	exit 1
fi

username=$1

sudo userdel -fr $username 2>&1 | grep -vi '/mail'
sudo groupdel $username 2>&1 | grep -vi 'does not exist'

exit 0
