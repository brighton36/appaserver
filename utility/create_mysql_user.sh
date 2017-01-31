:
# utility/create_mysql_user.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -ne 2 ]
then
	echo "Usage: $0 username password" 1>&2
	exit 1
fi

username=$1
password=$2

echo "create user '$username'@'localhost' identified by '$password';"|
sql.e

echo "grant all on $username.* to '$username'@'localhost';"		       |
sql.e

exit 0

