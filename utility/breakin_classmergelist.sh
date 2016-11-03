:
# ---------------------------------
# utility/breakin_classmergelist.sh
# ---------------------------------

classmergelist_C()
{
	most_frequent_head=$1

	breakin_classcheck.sh C				|
	head -${most_frequent_head}			|
	piece.e '|' 1					|
	joinlines.e ' '					|
	xargs.e breakin_classmerge.sh C {}
}

classmergelist_B()
{
	most_frequent_head=$1

	breakin_classcheck.sh B				|
	head -${most_frequent_head}			|
	piece.e '|' 1					|
	joinlines.e ' '					|
	xargs.e breakin_classmerge.sh B {}
}

exit_usage=0
class=C

if [ "$#" -eq 2 ]
then
	class=$2
elif [ "$#" -ne 1 ]
then
	exit_usage=1
fi

if [ "$class" != "B" -a "$class" != "b" -a "$class" != "C" -a "$class" != "c" ]
then
	exit_usage=1
fi

if [ $exit_usage -eq 1 ]
then
	echo "Usage: $0 most_frequent_head [B|C]" 1>&2
	exit 1
fi

most_frequent_head=$1

if [ $class = C ]
then
	classmergelist_C $most_frequent_head
else
	classmergelist_B $most_frequent_head
fi

exit 0
