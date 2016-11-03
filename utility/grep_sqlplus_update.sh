:
# grep_sqlplus_update.sh
# ----------------------
#
# Tim Riley
# ----------------------
grep=/usr/xpg4/bin/grep

$grep -e 'ERROR' -e 'updated' -e 'ORA'
exit $?
