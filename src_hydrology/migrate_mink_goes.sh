:
# migrate_mink_goes.sh
# --------------------

cd /opt/lrgs
nohup /usr/bin/time rsync -a --progress --delete . bonefish:/opt/lrgs >/var/tmp/migrate_mink_lrgs.out 2>&1 &

exit 0
