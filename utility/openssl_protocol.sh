:
# $APPASERVER_HOME/utility/openssl_protocol.sh
# -----------------------------------------------
# Freely available software: see Appaserver.org
# -----------------------------------------------

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 url" 1>&2
	exit 1
fi

url=$1

grep="Protocol  :"

echo									|
openssl s_client -connect ${url}:443 -servername $url 2>/dev/null	|
grep "$grep"								|
cat

exit 0
