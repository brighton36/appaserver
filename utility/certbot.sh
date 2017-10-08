:
# $APPASERVER_HOME/utility/certbot.sh
# ---------------------------------------------
# Freely available software: see Appaserver.org
# ---------------------------------------------

if [ "$#" -lt 3 ]
then
	echo "Usage: $0 operation domain document_root_leaf [testing]" 1>&2
	echo "operation = {create,delete}" 1>&2
	echo "Note: only run this if you're fulling using the Group Centric Paradigm."
	exit 1
fi

operation=$1
domain=$2
document_root_leaf=$3

if [ "$#" -eq 4 -a "$4" = "staging" ]
then
	testing=yes
fi

if [ "$#" -eq 4 -a "$4" = "testing" ]
then
	testing=yes
fi

function certbot_delete ()
{
	domain=$1
	testing=$2

	if [ "$testing" = "yes" ]
	then
		staging="--staging"
	fi

	cert_path="/etc/letsencrypt/live/${domain}/cert.pem"

	sudo certbot revoke --cert-path $cert_path $staging

	sudo certbot delete --cert-name $domain $staging
}

function certbot_create ()
{
	domain=$1
	document_root_leaf=$2
	testing=$3

	if [ "$testing" = "yes" ]
	then
		staging="--staging"
	fi

	webroot_path="${DOCUMENT_ROOT}/${document_root_leaf}"

	sudo certbot	certonly			\
			--webroot			\
			--webroot-path			\
			${webroot_path}			\
			-d $domain			\
			$staging

	keypath="/etc/letsencrypt/archive/${domain}"
	sudo chmod g+rwxs $keypath
	sudo chmod o-rx $keypath
	sudo chmod o-r $keypath/*
}

if [ "$operation" = "delete" ]
then
	certbot_delete $domain $testing
elif [ "$operation" = "create" ]
then
	certbot_create $domain $document_root_leaf $testing
else
	echo "$0: Invalid operation." 1>&2
	exit 1
fi

exit 0
