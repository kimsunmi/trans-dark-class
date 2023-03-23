#!/bin/zsh

FILENAME="./result_230117_2"

echo "" > ${FILENAME}
make clean_all
make all

echo "================ only precomputation ================" >> ${FILENAME}

for i in {10..14}
do
    echo "================ test n = $i ================"
    echo "================ test n = $i ================" >> ${FILENAME}
    echo "SETUP...."
    ./TRANS_Setup 2048 $i >> ${FILENAME}
    echo >>  ${FILENAME}
    echo "COMMIT...."
    ./TRANS_Commit >> ${FILENAME}
    echo >>  ${FILENAME}
    echo "OPEN...."
    
done