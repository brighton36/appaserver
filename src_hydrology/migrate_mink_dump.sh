:
# migrate_mink_dump.sh
# --------------------

# ==============
# Database dumps
# ==============

# Hydrology database
# ------------------
# vi /appaserver/cron/mysqldump_hydrology_block.dat
# vi> offsite_scp_destination=bonefish:/dfe/son

# Outputs to:
# 1) /opt/physical/data/dfe/backup_block/son/$date
# 2) bonefish:/dfe/son.
# 3) /var/tmp/mysqldump.out
# ------------------------------------------------
cd /appaserver/cron

nohup /usr/bin/time						\
	mysqldump_database.sh	hydrology			\
				mysqldump_hydrology_block.dat	\
> /var/tmp/mysqldump_hydrology.out 2>&1 &

# ^^^^^^^^^^^^^^^^^^^^^^
# Expect to take 1 hour.
# ----------------------

# Sparrow database
# ----------------
# vi /appaserver/src_sparrow/mysqldump_sparrow.dat
# vi> offsite_scp_destination=bonefish:/dfe/son

# Outputs to:
# 1) /opt/physical/data/dfe/sparrow/son/$date
# 2) bonefish:/dfe/son.
# 3) /appaserver/cron_log/mysqldump_sparrow.out
# ---------------------------------------------

cd /appaserver/src_sparrow

nohup /usr/bin/time						\
	mysqldump_database.sh	sparrow				\
				mysqldump_sparrow.dat		\
> /var/tmp/mysqldump_sparrow.out 2>&1 &

# ^^^^^^^^^^^^^^^^^^^^^^^^^
# Expect to take 5 seconds.
# -------------------------

exit 0
