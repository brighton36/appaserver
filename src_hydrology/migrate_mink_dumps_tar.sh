:
# migrate_mink_dumps_tar.sh
# -------------------------

# -----------------------------------------
# This is called from migrate_mink_dumps.sh
# -----------------------------------------

block_tar=backup_block.tar
line_tar=backup_line.tar
sparrow_tar=sparrow.tar

cd /opt/physical/data/dfe

tar cvf $block_tar backup_block
tar cvf $line_tar backup_line
tar cvf $sparrow_tar sparrow

exit 0
