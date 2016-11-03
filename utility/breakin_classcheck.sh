:
# -----------------------------
# utility/breakin_classcheck.sh
# -----------------------------

classcheck_C()
{
	sudo ufw status				|
	grep DENY				|
	grep 22					|
	column.e 2				|
	piece_inverse.e 3 '.'			|
	sort -n					|
	group.e					|
	sort -nr				|
	head -20
}

classcheck_B()
{
	sudo ufw status				|
	grep DENY				|
	grep 22					|
	column.e 2				|
	piece_inverse.e 3 '.'			|
	piece_inverse.e 2 '.'			|
	sort -n					|
	group.e					|
	sort -nr				|
	head -20
}

class=C
exit_usage=0

if [ "$#" -eq 1 ]
then
	class=$1
elif [ "$#" -ge 2 ]
then
	exit_usage=1
fi

if [ "$class" != "B" -a "$class" != "b" -a "$class" != "C" -a "$class" != "c" ]
then
	exit_usage=1
fi

if [ $exit_usage -eq 1 ]
then
	echo "Usage: $0 B|C" 1>&2
	exit 1
fi

if [ $class = C ]
then
	classcheck_C
else
	classcheck_B
fi

exit 0
