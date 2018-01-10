:
# migrate_bonefish_dumps.sh
# -------------------------

# Run this after migrate_mink_dumps.sh finishes.
# ----------------------------------------------

block_tar=backup_block.tar
line_tar=backup_line.tar
sparrow_tar=sparrow.tar

cd /opt/physical/data/dfe

nohup /usr/bin/time
(
tar xvf $block_tar
tar xvf $line_tar
tar xvf $sparrow_tar
) > /dfe/tmp/migrate_bonefish_dumps.out 2>&1 &

exit 0
