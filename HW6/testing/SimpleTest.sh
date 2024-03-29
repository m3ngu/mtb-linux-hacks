#! /bin/bash

#create dir
mkdir SuperEXT2
#mount
mount -t ext2 /dev/sdb1 SuperEXT2

#create files
cd SuperEXT2
touch temp1.txt
touch temp2.txt

#tag them
../tag temp1.txt add Thierry
../tag temp1.txt add Rocks
../tag temp2.txt add 'Ben and Mengu are fine too'
# show tags
../tag temp1.txt
../tag temp2.txt

# mount unmount
echo '************* MOUNT / UNMOUNT ****************'
cd ..
umount -t ext2 SuperEXT2
mount -t ext2 /dev/sdb1 SuperEXT2
cd SuperEXT2

# show tags again
../tag temp1.txt
../tag temp2.txt

# rm tags
../tag temp1.txt rm 'Rocks'
../tag temp1.txt

