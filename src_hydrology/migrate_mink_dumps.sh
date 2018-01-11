:
# migrate_mink_dumps.sh
# ---------------------

nohup /usr/bin/time
(
	migrate_mink_dumps_tar.sh
	migrate_mink_dumps_rsync.sh
) > /var/tmp/migrate_mink_dumps.out 2>&1 &

exit 0
