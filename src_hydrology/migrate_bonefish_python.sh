:
# migrate_bonefish_python.sh
# --------------------------

# Run this after migrate_mink.sh
# ------------------------------

cd /dfe/tmp
subtract_list.e python_mink.dat python_bonefish.dat	|
xargs.e sudo apt-get -y install {}

exit 0
