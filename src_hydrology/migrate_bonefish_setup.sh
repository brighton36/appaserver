:
# migrate_bonefish_setup.sh
# -------------------------

# Expect size:
# ------------
# /var/lib/mysql				  184G

# Document root
# -------------
sudo chown timriley:appaserver $DOCUMENT_ROOT
chmod g+wxs $DOCUMENT_ROOT
mkdir $DOCUMENT_ROOT/hydrology
mkdir $DOCUMENT_ROOT/hydrology/images

# Database dumps
# --------------
sudo mkdir /opt/physical
sudo chown timriley:appaserver /opt/physical
chmod g+wxs /opt/physical
mkdir /opt/physical/sql
mkdir /opt/physical/tmp2
mkdir /opt/physical/data
mkdir /opt/physical/data/dfe

cd /opt/physical/data/dfe
echo "This is the destination directory for nightly backup dumps." > README

mkdir /opt/physical/data/dfe/backup_block
cd /opt/physical/data/dfe/backup_block
mkdir son father grandfather greatgrandfather greatgreatgrandfather

mkdir /opt/physical/data/dfe/backup_line
cd /opt/physical/data/dfe/backup_line
mkdir son father grandfather greatgrandfather greatgreatgrandfather

mkdir /opt/physical/data/dfe/sparrow
cd /opt/physical/data/dfe/sparrow
mkdir son father grandfather greatgrandfather greatgreatgrandfather

cd /opt/physical/tmp2
echo "This directory contains the GOES satellite load bootstrap script." > README

# Expect sizes:
# -------------
# /opt/physical/data/dfe/backup_block		127G
# /opt/physical/data/dfe/backup_line		134G
# /opt/physical/data/dfe/sparrow		 29M

# Temporary migration directories
# -------------------------------
sudo mkdir /dfe
sudo chown timriley:appaserver /dfe
chmod g+wxs /dfe
mkdir /dfe/tmp
cd /dfe/tmp
echo "This is the migration temporary directory." > README
mkdir /dfe/son
cd /dfe/son
echo "This is the migration database storage directory." > README
mkdir /dfe/son/hydrology
cd /dfe/son/hydrology
echo "This is the hydrology migration database storage directory." > README
mkdir /dfe/son/sparrow
cd /dfe/son/sparrow
echo "This is the sparrow migration database storage directory." > README

# Old appaserver
# --------------
sudo mkdir /dfe/appaserver
sudo chown timriley:appaserver /dfe/appaserver
chmod g+wxs /dfe/appaserver

# These directories won't be copied from mink.
# --------------------------------------------
mkdir /dfe/appaserver/cron
mkdir /dfe/appaserver/cron/data/
mkdir /dfe/appaserver/cron/data/BISC

cd /dfe/appaserver
echo "This is old Appaserver directory." > README
echo "It's needed because scripts have this path hard coded." >> README
echo "Also, processes output to here. However, the new Appaserver" >> README
echo "directory can only contain source code and binaries." >> README

cd /
sudo ln -s /dfe/appaserver `pwd`

# Expect size:
# ------------
# /dfe/appaserver/cron/data/BISC		  70G

# GOES Satellite directory
# ------------------------
sudo mkdir /opt/lrgs
sudo chown timriley:appaserver /opt/lrgs
chmod g+wxs /opt/lrgs
cd /opt/lrgs
echo "This is the GOES satellite raw file directory." > README

# Make GOES data directories.
# These won't be copied from mink.
# -------------------------------
mkdir /opt/lrgs/data
mkdir /opt/lrgs/data/goes_archive
mkdir /opt/lrgs/data/goes_raw_archive
mkdir /opt/lrgs/data/load_archive
mkdir /opt/lrgs/data/load

# Expect size:
# ------------
# /opt/lrgs					  19G

# User scripts 1
# --------------
sudo mkdir /opt/dfe
sudo chown timriley:appaserver /opt/dfe
chmod g+wxs /opt/dfe
mkdir /opt/dfe/sql

cd /opt/dfe
echo "The sql directory contains user scripts." > README
echo "Other user scripts are in /opt/physical/sql and /usr/local/util." >> README
echo "Other entries are links to the old appaserver version for legacy processes." >> README

cd /opt/dfe/sql
echo "This directory contains user scripts." > README
echo "Other user scripts are in /opt/physical/sql and /usr/local/util." >> README

ln -s /dfe/appaserver/cron `pwd`
ln -s /dfe/appaserver/src_appaserver `pwd`
ln -s /dfe/appaserver/utility `pwd`

# User scripts 2
# --------------
sudo mkdir /usr/local/util
sudo chown timriley:appaserver /usr/local/util
chmod g+wxs /usr/local/util

cd /usr/local/util
echo "This directory contains user scripts." > README
echo "Other user scripts are in /opt/physical/sql and /opt/dfe/sql." >> README

sudo mkdir /usr2
sudo chown timriley:appaserver /usr2
chmod g+wxs /usr2
sudo ln -s /usr/local/util /usr2/lib

# User scripts 3
# --------------
cd /opt/physical
echo "The sql directory contains user scripts." > README
echo "Other user scripts are in /usr/local/util and /opt/dfe/sql." >> README

cd /opt/physical/sql
echo "This directory contains user scripts." > README
echo "Other user scripts are in /usr/local/util and /opt/dfe/sql." >> README

# CR10 directory
# --------------
sudo mkdir /var/export
sudo chown timriley:appaserver /var/export
chmod g+wxs /var/export
cd /var/export
echo "This directory contains the CR10 raw files." > README
mkdir enpdata enpdata_incoming dailyplots archive enpdata_done enpdata_done/archive temp enpdata_archive enpdata2
sudo ln -s /var/export /export

# Expect size:
# ------------
# /var/export					  42G

# Python libraries
# ----------------
dpkg -l							|
grep -i python						|
column.e 1						|
piece.e ':' 0 > /dfe/tmp/python_bonefish.dat

# Create user accounts
# --------------------
useradd.sh rondeau appaserver
sudo mkdir ~rondeau/LrgsClient
sudo chown rondeau:appaserver ~rondeau/LrgsClient
sudo chmod g+ws ~rondeau/LrgsClient

useradd.sh stabenau appaserver
