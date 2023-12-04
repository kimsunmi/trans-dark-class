#!/bin/bash

FILENAME="./1201_trans_class_2"

echo "" > ${FILENAME}
make clean_all
make all

echo "================ compile w/ optimization option -O2 ================" >> ${FILENAME}

for i in {7..10}
do
    echo "================ test n = $i ================"
    echo "================ test n = $i ================" >> ${FILENAME}
    echo "SETUP...."
    echo "SETUP...." >>  ${FILENAME}
    ./TRANS_Setup 512 $i >> ${FILENAME}
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
