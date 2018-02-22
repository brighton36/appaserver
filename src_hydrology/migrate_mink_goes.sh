:
# migrate_mink_goes.sh
# --------------------
# After this run migrate_bonefish_permissions.sh
# ----------------------------------------------

cd /opt/lrgs

nohup /usr/bin/time					\
	rsync	-a					\
		--progress				\
		--delete				\
		--hard-links				\
		--exclude=data				\
		.					\
		bonefish:/opt/lrgs			\
> /var/tmp/migrate_mink_lrgs.out 2>&1 &

#cd /dfe/appaserver/cron/data

#nohup /usr/bin/time					\
#	rsync	-a					\
#		--progress				\
#		--delete				\
#		.					\
#		bonefish:/dfe/appaserver/cron/data	\
#> /var/tmp/migrate_mink_cron_data.out 2>&1 &

exit 0
