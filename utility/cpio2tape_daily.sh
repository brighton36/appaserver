:
# cpio2tape_daily.sh
# ------------------
device=/dev/nht0
username=timriley

cd /usrb2/appaserver/utility
mt -f $device rewind
cpio2tape_mysql_datafiles.sh y
cpio2tape_appaserver.sh y
cpio2tape_home_directory.sh $username y
cpio2tape_etc.sh y
mt -f $device rewind
