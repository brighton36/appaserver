:
# migrate_mink.sh
# ---------------

set -x

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

# =========
# Databases
# =========
# Hydrology database
# ------------------
# vi /appaserver/cron/mysqldump_hydrology_block.dat
# vi> offsite_scp_destination=bonefish:/dfe/son

# Outputs to:
# 1) /opt/physical/data/dfe/backup_block/son/$date
# 2) bonefish:/dfe/son.
# 3) /var/tmp/mysqldump.out
# ------------------------------------------------
# cd /appaserver/cron
# nohup /usr/bin/time mysqldump_database.sh hydrology mysqldump_hydrology_block.dat >/var/tmp/mysqldump.out 2>&1 &

# Sparrow database
# ----------------
# vi /appaserver/src_sparrow/mysqldump_sparrow.dat
# vi> offsite_scp_destination=bonefish:/dfe/son

# Outputs to:
# 1) /opt/physical/data/dfe/sparrow/son/$date
# 2) bonefish:/dfe/son.
# 3) /appaserver/cron_log/mysqldump_sparrow.out
# ---------------------------------------------
# cd /appaserver/src_sparrow
# nohup /usr/bin/time mysqldump_database.sh hydrology mysqldump_sparrow.dat >/appaserver/cron_log/mysqldump_sparrow.out 2>&1 &

# Old appaserver
# --------------
cd /dfe/appaserver
rm data/*
rm sparrow/appaserver/*[1-9][0-9]*

/usr/bin/time rsync	-a				\
			--progress			\
			--delete			\
			--exclude=cron/data/		\
			. bonefish:/dfe/appaserver

# User scripts 1
# --------------
cd /opt/dfe/sql
/usr/bin/time rsync -a --progress --delete .  bonefish:/opt/dfe/sql

# User scripts 2
# --------------
cd /usr/local/util
/usr/bin/time rsync -a --progress --delete . bonefish:/usr/local/util

# User scripts 3
# --------------
cd /opt/physical/sql
/usr/bin/time rsync -a --progress --delete . bonefish:/opt/physical/sql

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
/usr/bin/time rsync -a --progress --delete . bonefish:/opt/physical/tmp2

# Database dumps
# --------------
# migrate_mink_dumps.sh

# GOES Satellite files
# --------------------
# migrate_mink_goes.sh

# CR10 directory
# --------------
# migrate_mink_cr10.sh

exit 0
