:
# migrate_mink_goes.sh
# --------------------

cd /opt/lrgs
/usr/bin/time rsync -a --progress delete . bonefish:/opt/lrgs

exit 0
