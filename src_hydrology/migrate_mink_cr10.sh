:
# migrate_mink_cr10.sh
# --------------------

# Expecte to take 2 1/2 hours.
# ----------------------------

cd /var/export
nohup /usr/bin/time				\
	rsync	-a				\
		--progress			\
		--delete			\
		--hard-links			\
		.				\
		bonefish:/var/export		\
> /var/tmp/migrate_mink_cr10.out 2>&1 &

exit 0
