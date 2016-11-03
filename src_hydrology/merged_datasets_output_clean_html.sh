:
if [ "$#" -ne 2 ]
then
	echo "" 1>&2
	echo "Usage: $0 delimiter datatype_count" 1>&2
	echo "" 1>&2
	echo "Store the output to merged_datasets_output to a file," 1>&2
	echo "then execute this like" 1>&2
	echo "$0 '|' 2 < that_file.html" 1>&2
	echo "" 1>&2
	exit
fi

delimiter=$1
datatype_count=$2

number_joins=`expr $datatype_count + 1`
	
dos2unix						|
grep -v 'Content'					|
grep -v '<html'						|
grep -v '<link'						|
grep -v '<TITLE'					|
grep -v '<body'						|
grep -v '<table'					|
grep -v '<caption'					|
grep -v '<tr>'						|
sed 's|<td align=right></td>|null|'			|
sed 's/<th align=left>//'				|
sed 's/<th align=right>//'				|
sed 's/<td align=left>//'				|
sed 's/<td align=right>//'				|
sed 's|</th>||'						|
sed 's|</td>||'						|
sed 's|</html>||'					|
sed 's|</body>||'					|
sed 's|</table>||'					|
sed 's|</head>||'					|
grep -v '^$'						|
joinlines.e "$delimiter" $number_joins			|
merged_datasets_remove_all_but_first_heading.e 		|
cat
