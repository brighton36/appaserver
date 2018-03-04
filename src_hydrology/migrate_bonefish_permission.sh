:
# migrate_bonefish_permission.sh
# ------------------------------

# Run this after migrate_mink_goes.sh
# -----------------------------------

cd /opt/lrgs

find . -type d -exec sudo chgrp appaserver {} \;
find . -type d -exec sudo chmod g+ws {} \;

find . -type f -exec sudo chgrp appaserver {} \;
find . -type f -exec sudo chmod g+w {} \;

cd ~rondeau/LrgsClient

find . -exec sudo chown rondeau:appaserver {} \;
find . -type f -exec sudo chmod g+w {} \;
find . -type d -exec sudo chmod g+ws {} \;

cd ~rondeau/util

find . -exec sudo chown rondeau:appaserver {} \;
find . -type f -exec sudo chmod g+w {} \;
find . -type d -exec sudo chmod g+ws {} \;

cd /opt/physical/data

find . -exec sudo chgrp appaserver {} \;
find . -type f -exec sudo chmod g+w {} \;
find . -type d -exec sudo chmod g+ws {} \;

exit 0
