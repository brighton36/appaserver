:
# ------------------
# utility/breakin.sh
# ------------------
current_ip_address=`current_ip_address.sh 2>/dev/null`
echo "Current IP Address: $current_ip_address"
with_deny_process="cat"

if [ "$#" -eq 1 ]
then
	if [ "$1" = "withdeny" ]
	then
		with_deny_process="xargs.e sudo ufw insert 1 deny to any port 22 from {}"
	else
		echo "Usage $0 [withdeny]" 1>&2
		exit 1
	fi
fi

(
sudo grep 'Failed password for root' /var/log/auth.log		|
column.e 10							|
cat								;
sudo grep 'Failed password for invalid' /var/log/auth.log	|
column.e 12							|
cat
)								|
sort -u								|
grep -v '^;'							|
grep -v "$current_ip_address"					|
breakin_whitelist.e						|
$with_deny_process						|
cat

if [ "$with_deny_process" != "cat" ]
then
	sudo -s 'echo > /var/log/auth.log'
fi

exit 0
