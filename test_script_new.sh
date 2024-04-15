#!/bin/bash

FILENAME="./class_test_result.txt"

for i in {12..13}
do
    echo " ==== test n = $i ==== "
    echo " ==== test n = $i ==== " >> ${FILENAME}
    echo "Setup..."
    echo "Setup..." >> ${FILENAME}
    ./TRANS_Setup 512 $i >> ${FILENAME}
    echo >> ${FILENAME}
    echo "TEST..."
    echo "TEST..." >> ${FILENAME}
    ./TRANS_Test >> ${FILENAME}
    echo >> ${FILENAME}
done
: << "END"

#!/bin/bash

FILENAME="./test_for_pp"

echo "" > ${FILENAME}
make clean_all
make all

echo "================ compile w/ optimization option -O2 ================" >> ${FILENAME}

for i in {7..110}
do
    echo "================ test n = $i ================"
    echo "================ test n = $i ================" >> ${FILENAME}
    echo "SETUP...."
    echo "SETUP...." >>  ${FILENAME}
    ./TRANS_Setup 2048 $i >> ${FILENAME}
    echo >> ${FILENAME}
    echo "OPEN...."
    echo "OPEN...." >>  ${FILENAME}
    ./TRANS_Open >> ${FILENAME}
    echo >>  ${FILENAME}
    echo "VERIFY...."
    echo "VERIFY...." >>  ${FILENAME}
    ./TRANS_Verify >> ${FILENAME}
    echo >>  ${FILENAME}
done
END