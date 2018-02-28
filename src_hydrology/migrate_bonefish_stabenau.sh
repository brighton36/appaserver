:
# migrate_bonefish_stabenau.sh
# ----------------------------

# Run this after migrate_mink_goes.sh
# -----------------------------------

cd ~stabenau

sudo cp /dfe/tmp/stabenau/* .

find . -type f -exec sudo chown stabenau:appaserver {} \;
find . -type f -exec sudo chmod g+w {} \;

exit 0
