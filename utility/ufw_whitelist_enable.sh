:
# ------------------------------------------------
# $APPASERVER_HOME/utility/ufw_whitelist_enable.sh
# ------------------------------------------------
# sudo ufw app list
# sudo lsof -i -nP
# sudo netstat -p
# siege $target

ufw_whitelist.e				|
xargs.e sudo ufw allow from {}		|
cat

exit 0
