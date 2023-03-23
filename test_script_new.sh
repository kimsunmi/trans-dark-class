#!/bin/zsh

FILENAME="./result_final_th1"

echo "" > ${FILENAME}
make clean_all
make all

echo "================ compile w/ optimization option -O2 ================" >> ${FILENAME}

for i in {10..16}
do
    echo "================ test n = $i ================"
    echo "================ test n = $i ================" >> ${FILENAME}
    echo "SETUP...."
    echo "SETUP...." >>  ${FILENAME}
    ./TRANS_Setup 2048 $i >> ${FILENAME}
    echo >>  ${FILENAME}
    echo "COMMIT...."
    echo "COMMIT...." >>  ${FILENAME}
    ./TRANS_Commit >> ${FILENAME}
    echo >>  ${FILENAME}
    echo "OPEN...."
    echo "OPEN...." >>  ${FILENAME}
    ./TRANS_Open >> ${FILENAME}
    echo >>  ${FILENAME}
    echo "VERIFY...."
    echo "VERIFY...." >>  ${FILENAME}
    ./TRANS_Verify >> ${FILENAME}
    echo >>  ${FILENAME}
done