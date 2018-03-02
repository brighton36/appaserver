:
# migrate_mink.sh
# ---------------

# Damon's scripts
# ---------------
directory="/home/rondeau/LrgsClient"
rsync -az --progress $directory bonefish:$directory

directory="/home/rondeau/util"
rsync -az --progress $directory bonefish:$directory

# Eric's scripts
# --------------
scp ~stabenau/set* bonefish:/dfe/tmp/stabenau

# Dependent data files
# --------------------
directory="/opt/physical/data/raw_data"
rsync -az --progress $directory bonefish:$directory

directory="/opt/physical/data/survey_field_sheets"
rsync -az --progress $directory bonefish:$directory

directory="/var/export/enpdata/dailyplots"
scp $directory/msg.txt bonefish:$directory

#directory="/opt/physical/data/requests"
#rsync	-az					\
#	--progress				\
#	--exclude=audubon/sent			\
#	--exclude=amy				\
#	--exclude=bisc				\
#	--exclude=engel				\
#	--exclude=lter				\
#	--exclude=eden				\
#	--exclude=daily_values			\
#	--exclude=sfwmd				\
#	--exclude=sfwmd_hourly			\
#	--exclude=simeon			\
#	--exclude=kirenbisc			\
#	--exclude=jemeema			\
#	--exclude=troxler			\
#	$directory				\
#	bonefish:$directory

# Utilities
# ---------
directory="/opt/physical/data/requests/audubon/util"
rsync -az --progress $directory bonefish:$directory

directory="/opt/physical/data/dailyplots/util"
rsync -az --progress $directory bonefish:$directory

directory="/opt/physical/data/fathom_sal/util"
rsync -az --progress $directory bonefish:$directory

directory="/opt/physical/data/requests/util"
rsync -az --progress $directory bonefish:$directory

directory="/opt/physical/data/uvnet/util"
rsync -az --progress $directory bonefish:$directory

directory="/opt/physical/data/usgs_load/util"
rsync -az --progress $directory bonefish:$directory

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
