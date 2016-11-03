:
# sqlplus.sh
# ----------------

if [ "$#" -lt 1 ]
then
	echo $0 "uid/pwd [delimiter]" 1>&2
	exit 1
fi

uid_pwd=$1

if [ $# -eq 2 ]
then
	delimiter=$2
else
	delimiter="^"
fi

. `get_appaserver_mount_point`/src_appaserver/profile_oracle.sh

sed 's/\\\$/$/g'						|
select2delimiter.e "$delimiter"					|
sqlplus -s $uid_pwd						|
egrep '(^\^|^ORA-[0-9])'					|
sed 's/unique constraint/duplicate/'				|
# --------------------						|
# trim the first carot						|
# --------------------						|
sed 's/^\^//'							|
# ---------------------------					|
# trim spaces after delimiter					|
# ---------------------------					|
sed "s/[	 ][ 	]*${delimiter}/${delimiter}/g"		|
cat

exit 0
