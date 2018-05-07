:
# migrate_destination_stabenau.sh
# -------------------------------

# Run this after migrate_source_goes.sh
# -------------------------------------

cd ~stabenau

sudo cp /dfe/tmp/stabenau/* .

find . -type f -exec sudo chown stabenau:appaserver {} \;
find . -type f -exec sudo chmod g+w {} \;

exit 0
