:
# migrate_source_goes.sh
# ----------------------
# After this run migrate_destination_permissions.sh
# -------------------------------------------------

cd /opt/lrgs

nohup /usr/bin/time					\
	rsync	-a					\
		--progress				\
		--delete				\
		--hard-links				\
		--exclude=data				\
		.					\
		bonefish:/opt/lrgs			\
> /var/tmp/migrate_source_lrgs.out 2>&1 &

#cd /dfe/appaserver/cron/data

#nohup /usr/bin/time					\
#	rsync	-a					\
#		--progress				\
#		--delete				\
#		.					\
#		bonefish:/dfe/appaserver/cron/data	\
#> /var/tmp/migrate_source_cron_data.out 2>&1 &

exit 0
