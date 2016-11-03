:
# -----------------------
# utility/breakin_grep.sh
# -----------------------
breakin.sh						|
grep "^[1-9]" 						|
xargs.e "cat /var/log/auth.log | grep {} | less"

exit 0
