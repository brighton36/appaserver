:
# utility/userappaserver.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 username" 1>&2
	exit 1
fi

if [ "$CGI_HOME" = "" ]
then
	echo "Error: CGI_HOME not defined." 1>&2
	exit 1
fi

if [ "$APPASERVER_HOME" = "" ]
then
	echo "Error: APPASERVER_HOME not defined." 1>&2
	exit 1
fi

username=$1
home_directory=/home/$username
cgi_directory=$home_directory/cgi-bin/
local_appaserver=$home_directory/public_html/appaserver
local_cgi=$home_directory/cgi-bin
local_appaserver_config=$local_cgi/appaserver.config

if [ ! -d $APPASERVER_HOME/$username ]
then
	echo "Error: $APPASERVER_HOME/$username doesn't exists." 1>&2
	exit 1
fi

cd $APPASERVER_HOME
sudo chown $username $username
sudo chmod o-rx $username

sudo ln -s $APPASERVER_HOME/$username $local_appaserver
sudo ln -s $APPASERVER_HOME/zmenu $local_appaserver
sudo ln -s $APPASERVER_HOME/javascript $local_appaserver
sudo ln -s $APPASERVER_HOME/$username/index.php $local_appaserver
sudo ln -s $APPASERVER_HOME/$username/style.css $local_appaserver

for file in `ls -1 $APPASERVER_HOME/*gif`
do
	sudo ln -s $file $local_appaserver
done

for file in `ls -1 $CGI_HOME/*`
do
	sudo ln -s $file $cgi_directory
done

# Make the local appaserver.config
# --------------------------------
sudo cp /etc/appaserver.config $local_appaserver_config
sudo chmod g+rw $local_appaserver_config
sudo chmod o-rw $local_appaserver_config
sudo chown $username:$username $local_appaserver_config

# Populate the local directory
# ----------------------------
cd $local_appaserver
sudo ln -s $APPASERVER_HOME/$username/images `pwd`
sudo ln -s $DOCUMENT_ROOT/favicon.ico `pwd`

# Touch up the error file
# -----------------------
cd `get_appaserver_error_directory`
sudo chown $username appaserver_${username}.err

exit 0

