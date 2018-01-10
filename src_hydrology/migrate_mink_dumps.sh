:
# migrate_mink_dumps.sh
# ---------------------

block_tar=backup_block.tar
line_tar=backup_line.tar
sparrow_tar=sparrow.tar

cd /opt/physical/data/dfe

nohup /usr/bin/time
(
tar cvf $block_tar backup_block
tar cvf $line_tar backup_line
tar cvf $sparrow_tar sparrow
rsync -a --progress $block_tar $line_tar $sparrow_tar bonefish:/opt/physical/data/dfe
) > /var/tmp/migrate_mink_dumps.out 2>&1 &

exit 0
