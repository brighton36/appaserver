:
# migrate_mink_cr10.sh
# --------------------

cd /var/export
nohup /usr/bin/time rsync -a --progress --delete . bonefish:/var/export >/var/tmp/migrate_mink_cr10.out 2>&1 &

exit 0
