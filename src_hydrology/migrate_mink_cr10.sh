:
# migrate_mink_cr10.sh
# --------------------

cd /var/export
/usr/bin/time rsync -a --progress --delete . bonefish:/var/export

exit 0
