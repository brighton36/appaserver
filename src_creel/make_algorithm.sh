:
rm species_estimated_total_caught_algorithm.pdf 2>/dev/null
rm species_estimated_total_caught_algorithm.log 2>/dev/null
rm species_estimated_total_caught_algorithm.aux 2>/dev/null
#pdflatex species_estimated_total_caught_algorithm.tex >/dev/null 2>&1 </dev/null
pdflatex species_estimated_total_caught_algorithm.tex

results=$?

if [ "$results" -eq 0 ]
then
	echo "Created species_estimated_total_caught_algorithm.pdf"
else
	echo "Error occurred."
fi

exit 0
