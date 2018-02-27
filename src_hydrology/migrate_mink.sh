:
# migrate_mink.sh
# ---------------

# Damon's scripts
# ---------------
scp -r ~rondeau/LrgsClient/* bonefish:~rondeau/LrgsClient
scp -r ~rondeau/util/* bonefish:~rondeau/util

# Dependent data files
# --------------------
scp -r /opt/physical/data/raw_data/* bonefish:/opt/physical/data/raw_data
scp -r /opt/physical/data/survey_field_sheets/* bonefish:/opt/physical/data/survey_field_sheets
scp -r /opt/physical/data/requests/* bonefish:/opt/physical/data/requests

# Database dumps
# --------------
migrate_mink_dump.sh

# Document root
# -------------
cd $DOCUMENT_ROOT/hydrology
scp index.* bonefish:/var/www/hydrology
scp *.css bonefish:/var/www/hydrology
scp images/* bonefish:/var/www/hydrology/images

# Crontabs
# --------
scp ~rondeau/cron_rondeau.dat bonefish:/dfe/tmp
scp ~kotun/cron_kotun.dat bonefish:/dfe/tmp

# Old appaserver
# --------------
cd /dfe/appaserver
rm -f data/*
rm -f sparrow/appaserver/*[1-9][0-9]*

/usr/bin/time rsync	-a				\
			--progress			\
			--exclude=cron/data/BISC	\
			. bonefish:/dfe/appaserver

# User scripts 1
# --------------
cd /opt/dfe/sql
/usr/bin/time rsync -a --progress . bonefish:/opt/dfe/sql

# User scripts 2
# --------------
cd /usr/local/util
/usr/bin/time rsync -a --progress . bonefish:/usr/local/util

# User scripts 3
# --------------
cd /opt/physical/sql
/usr/bin/time rsync -a --progress . bonefish:/opt/physical/sql

# Python libraries
# ----------------
dpkg -l						|
grep -i python					|
column.e 1					|
piece.e ':' 0 > /var/tmp/python_mink.dat

# Following this, execute migrate_bonefish_python.sh
# --------------------------------------------------
scp /var/tmp/python_mink.dat bonefish:/dfe/tmp

# GOES executables
# ----------------
cd /opt/physical/tmp2
/usr/bin/time rsync -a --progress . bonefish:/opt/physical/tmp2

# GOES Satellite files.
# This excludes data.
# --------------------
migrate_mink_goes.sh

# Database dumps <-- not doing
# ----------------------------
# migrate_mink_dumps.sh

# CR10 directory <-- not doing
# ----------------------------
# migrate_mink_cr10.sh

exit 0
