:
# generate_password.sh
# --------------------

if [ "$#" -eq 1 ]
then
	how_many=$1
else
	how_many=1
fi

word_file=/usr/share/dict/words
word_file_length=`cat $word_file | wc -l`

while [ $how_many -gt 0 ]
do
	random_number=`random.e ${word_file_length}`
	first_word=`head -$random_number $word_file | tail -1`
	
	sleep 1
	middle_number=`random.e 99`
	
	random_number=`random.e $word_file_length`
	second_word=`head -$random_number $word_file | tail -1`
	
	echo ${first_word}${middle_number}${second_word}	|
	tolower.e						|
	sed "s/'//g"

	how_many=`expr $how_many - 1`
done

exit 0
