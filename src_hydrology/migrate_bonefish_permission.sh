:
# migrate_bonefish_permission.sh
# ------------------------------

cd /opt/lrgs

find . -type d -exec chgrp appaserver {} \;
find . -type d -exec chmod g+ws {} \;

find . -type f -exec chgrp appaserver {} \;
find . -type f -exec chmod g+w {} \;

cd ~rondeau/LrgsClient

find . -type d -exec sudo chown rondeau:appaserver {} \;
find . -type d -exec sudo chmod g+ws {} \;

find . -type f -exec sudo chown rondeau:appaserver {} \;
find . -type f -exec sudo chmod g+w {} \;

exit 0
