:
# migrate_bonefish.sh
# -------------------

# Expect size:
# ------------
# /var/lib/mysql				  184G

# Document root
# -------------
sudo chown timriley:appaserver $DOCUMENT_ROOT
chmod g+wxs $DOCUMENT_ROOT
mkdir $DOCUMENT_ROOT/hydrology
mkdir $DOCUMENT_ROOT/hydrology/images

# Database backups
# ----------------
sudo mkdir /opt/physical
sudo chown timriley:appaserver /opt/physical
chmod g+wxs /opt/physical
mkdir /opt/physical/sql
mkdir /opt/physical/tmp2
mkdir /opt/physical/data
mkdir /opt/physical/data/dfe
mkdir /opt/physical/data/dfe/backup_block
mkdir /opt/physical/data/dfe/backup_line
mkdir /opt/physical/data/dfe/sparrow
cd /opt/physical/data
echo "This is the destination directory for nightly backup dumps." > README

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
echo "This is the migration temporary directory" > README
mkdir /dfe/son
cd /dfe/son
echo "This is the migration database storage directory" > README
mkdir /dfe/son/hydrology
cd /dfe/son/hydrology
echo "This is the migration database storage directory" > README

# Old appaserver
# --------------
sudo mkdir /dfe/appaserver
sudo chown timriley:appaserver /dfe/appaserver
chmod g+wxs /dfe/appaserver
cd /dfe/appaserver
echo "This is old Appaserver directory." > README

cd /
sudo ln -s /dfe/appaserver `pwd`

# Expect size:
# ------------
# /dfe/appaserver				  74G

# GOES Satellite files
# --------------------
sudo mkdir /opt/lrgs
sudo chown timriley:appaserver /opt/lrgs
chmod g+wxs /opt/lrgs
cd /opt/lrgs
echo "This is the GOES satellite directory." > README

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
echo "Other entries are links to old appaserver versions for legacy processes." >> README
ln -s /dfe/appaserver/cron `pwd`
ln -s /appaserver/src_appaserver `pwd`
ln -s /appaserver/utility `pwd`

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
echo "Other user scripts are in /opt/physical/sql and /opt/dfe/sql." >> README
cd /opt/physical/sql
echo "This directory contains user scripts." > README
echo "Other user scripts are in /opt/physical/sql and /opt/dfe/sql." >> README

# CR10 directory
# --------------
sudo mkdir /var/export
sudo chown timriley:appaserver /var/export
chmod g+wxs /var/export
cd /var/export
echo "This directory contains the CR10 files." > README

# Expect size:
# ------------
# /var/export					  42G

# Python libraries
# ----------------
dpkg -l							|
grep -i python						|
column.e 1						|
piece.e ':' 0 > /dfe/tmp/python_bonefish.dat

# GOES executables
# ----------------
sudo mkdir /opt/physical
sudo chown timriley:appaserver /opt/physical
chmod g+wxs /opt/lrgs
cd /opt/lrgs
echo "This is the GOES satellite directory." > README

