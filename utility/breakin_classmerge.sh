:
# -----------------------------
# utility/breakin_classmerge.sh
# -----------------------------

classmerge_C()
{
	network=$1

	sudo ufw status						|
	column.e 2						|
	grep "^$network"					|
	xargs.e sudo ufw delete deny to any port 22 from {}

	entry=${network}.0/24
	sudo ufw insert 1 deny to any port 22 from $entry
}

classmerge_B()
{
	network=$1

	sudo ufw status						|
	column.e 2						|
	grep "^$network"					|
	xargs.e sudo ufw delete deny to any port 22 from {}

	entry=${network}.0.0/16
	sudo ufw insert 1 deny to any port 22 from $entry
}

exit_usage=0

if [ "$#" -lt 2 ]
then
	exit_usage=1
fi

class=$1
shift

if [ "$class" != "B" -a "$class" != "b" -a "$class" != "C" -a "$class" != "c" ]
then
	exit_usage=1
fi

if [ $exit_usage -eq 1 ]
then
	echo "Usage: $0 B|C #.#[.#] [#.#.#]..." 1>&2
	exit 1
fi

while [ "$#" -gt 0 ]
do
	if [ $class = C -o $class = c ]
	then
		classmerge_C $1
	else
		classmerge_B $1
	fi

	shift
done

exit 0
