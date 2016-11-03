:
# utility/useradd.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -lt 1 ]
then
	echo "Usage: $0 username [group]" 1>&2
	exit 1
fi

if [ "$APPASERVER_HOME" = "" ]
then
	echo "Error: APPASERVER_HOME not defined." 1>&2
	exit 1
fi

username=$1

if [ "$#" -eq 2 ]
then
	group=$2
fi

home_directory=/home/$username
cgi_directory=$home_directory/cgi-bin/

# Create the username
# -------------------
if [ "$group" = "" ]
then
	sudo useradd -m $username
else
	sudo useradd -m -G $group $username
fi

sudo passwd $username

# Manage the home directory and subdirectories
# --------------------------------------------
sudo chmod g+ws $home_directory
sudo mkdir $home_directory/public_html
sudo chmod g+ws $home_directory/public_html
sudo chown $username $home_directory/public_html

sudo mkdir $home_directory/public_html/appaserver
sudo chown $username $home_directory/public_html/appaserver
sudo chmod g+ws $home_directory/public_html/appaserver
sudo chmod o-rw $home_directory/public_html/appaserver
sudo chmod o+x $home_directory/public_html/appaserver

sudo mkdir $cgi_directory
sudo chmod g+ws $cgi_directory
sudo chmod o-rws $cgi_directory
sudo chown $username $cgi_directory

exit 0

