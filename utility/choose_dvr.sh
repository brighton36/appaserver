:
transaction_file=/tmp/choose.out
input_file=/etc/passwd
hide_fields=0
delimiter=':'

if [ "$#" -ne 1 ]
then
	echo "Usage: $0 search"
	exit 1
fi

choose_one.py 	inputfile=$input_file 		\
		resultfile=$transaction_file	\
		hide_fields=$hide_fields	\
		delimiter=$delimiter		\
		search=$1			\
		debug=yes
cat $transaction_file
