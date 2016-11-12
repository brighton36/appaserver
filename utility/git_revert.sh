:
# utility/git_add.sh
# ------------------
git status				|
grep 'modified: '			|
column.e 1				|
xargs.e git checkout -- {}		|
cat

exit 0
