:
# migrate_destination_python.sh
# -----------------------------

# Run this after migrate_source.sh
# ---------------------------------

sudo apt-get -y install python3-mpltoolkits.basemap
sudo apt-get -y install python3-pandas
sudo apt-get -y install python3-geopandas

#https://github.com/matplotlib/natgrid
#Change to the natgrid directory and then:
#
#sudo python3 setup.py install

cd /dfe/tmp
subtract_list.e python_source.dat python_destination.dat	|
xargs.e sudo apt-get -y install {}

exit 0
