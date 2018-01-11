:
# migrate_mink_dumps_rsync.sh
# ---------------------------

# -----------------------------------------
# This is called from migrate_mink_dumps.sh
# -----------------------------------------

#block_tar=backup_block.tar
#line_tar=backup_line.tar
#sparrow_tar=sparrow.tar

cd /opt/physical/data/dfe

#rsync -a --progress $block_tar $line_tar $sparrow_tar bonefish:/opt/physical/data/dfe

rsync	-a				\
	--progress			\
	--delete			\
	--hard-links			\
	backup_line			\
	backup_block			\
	sparrow				\
	bonefish:/opt/physical/data/dfe

exit 0
