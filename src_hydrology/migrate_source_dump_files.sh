:
# migrate_source_dump_files.sh
# ----------------------------

# Not doing
# ---------
exit 0

cd /opt/physical/data/dfe

nohup /usr/bin/time				\
	rsync	-a				\
		--progress			\
		--delete			\
		--hard-links			\
		backup_block			\
		backup_line			\
		sparrow				\
		bonefish:/opt/physical/data/dfe	\
> /var/tmp/migrate_source_dumps.out 2>&1 &

exit 0
