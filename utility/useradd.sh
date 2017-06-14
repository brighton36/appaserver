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

home_directory=/home/$username
cgi_directory=$home_directory/cgi-bin/

# Create the username and default group
# -------------------------------------
sudo useradd -m $username
sudo addgroup $username
sudo usermod -a -G $username $username

# Assign to group
# ---------------
if [ "$group" != "" ]
then
	sudo addgroup $group
	sudo usermod -a -G $group $username
fi

sudo passwd $username

# Manage the home directory and subdirectories
# --------------------------------------------
#sudo chmod g+ws $home_directory
sudo mkdir $home_directory/public_html
sudo chown ${username}:${username} $home_directory/public_html
sudo chmod g+ws $home_directory/public_html

sudo mkdir $home_directory/public_html/appaserver
sudo chown ${username}:${username} $home_directory/public_html/appaserver
sudo chmod g+ws $home_directory/public_html/appaserver
sudo chmod o-rw $home_directory/public_html/appaserver
sudo chmod o+x $home_directory/public_html/appaserver

sudo mkdir $cgi_directory
sudo chown ${username}:${username} $cgi_directory
sudo chmod g+ws $cgi_directory
sudo chmod o-rws $cgi_directory

exit 0

