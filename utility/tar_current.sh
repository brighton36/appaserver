:
stamp=`current_date_backward.sh`
base=`pwd | xargs basename {}`
tar_file=/tmp/${base}_${stamp}.tar

tar cvf $tar_file *

echo
echo "Created $tar_file"
