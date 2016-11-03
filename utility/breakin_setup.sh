:
# ------------------------
# utility/breakin_setup.sh
# ------------------------
# sudo ufw app list
# sudo lsof -i -nP
# sudo netstat -p
# siege $target

rules="/etc/ufw/before.rules"

# http://blog.lavoie.sl/2012/09/protect-webserver-against-dos-attacks.html
# ------------------------------------------------------------------------
insert_http_identifiers()
{
temp="/tmp/ufw_$$"

cat $rules			|
sed 's/^COMMIT/#COMMIT/' > $temp
cat $temp > $rules

cat $rules			|
sed 's/^*filter/&\n:ufw-http - [0:0]\n:ufw-http-logdrop - [0:0]/' > $temp
cat $temp > $rules

rm -f $temp
}

append_http_limit()
{

cat >> $rules << all_done1

### Start HTTP ###
 
# Enter rule
-A ufw-before-input -p tcp --dport 80 -j ufw-http
-A ufw-before-input -p tcp --dport 443 -j ufw-http
 
# Limit connections per Class C
-A ufw-http -p tcp --syn -m connlimit --connlimit-above 50 --connlimit-mask 24 -j ufw-http-logdrop
 
# Limit connections per IP
-A ufw-http -m state --state NEW -m recent --name conn_per_ip --set
-A ufw-http -m state --state NEW -m recent --name conn_per_ip --update --seconds 10 --hitcount 20 -j ufw-http-logdrop
 
# Limit packets per IP
-A ufw-http -m recent --name pack_per_ip --set
-A ufw-http -m recent --name pack_per_ip --update --seconds 1 --hitcount 20 -j ufw-http-logdrop
 
# Finally accept
-A ufw-http -j ACCEPT
 
# Log-A ufw-http-logdrop -m limit --limit 3/min --limit-burst 10 -j LOG --log-prefix "[UFW HTTP DROP] "
-A ufw-http-logdrop -j DROP
 
### End HTTP ###
 
COMMIT

all_done1

}

rules_file_no_write()
{
	sudo chown root $rules
}

rules_file_write()
{
	sudo chown `whoami` $rules
}

remote_ip=`echo ${SSH_CONNECTION} | column.e 0`
sudo ufw disable
echo 'y' | sudo ufw reset >/dev/null
sudo ufw logging low
sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw allow from $remote_ip
sudo ufw allow from 127.0.0.1
sudo ufw allow http
sudo ufw allow https
sudo ufw allow domain
sudo ufw allow smtp
sudo ufw allow time
sudo ufw allow sftp
sudo ufw allow remoteping
sudo ufw allow ssh
sudo ufw limit ssh
# sudo ufw allow nfs

# Doesn't work :-(
# ----------------
# rules_file_write
# insert_http_identifiers
# append_http_limit
# rules_file_no_write

echo 'y' | sudo ufw enable >/dev/null

exit 0
