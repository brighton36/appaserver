:
rm species_estimated_total_caught_algorithm.pdf
rm species_estimated_total_caught_algorithm.log
rm species_estimated_total_caught_algorithm.aux
pdflatex species_estimated_total_caught_algorithm.tex >/dev/null 2>&1 </dev/null
#pdflatex species_estimated_total_caught_algorithm.tex

results=$?

if [ "$results" -eq 0 ]
then
	echo "Created species_estimated_total_caught_algorithm.pdf"
else
	echo "Error occurred."
fi

exit 0
