#!/bin/zsh
#
#(687 798 1208 1348 1665 1827 3040 4500 6784)
lambda=(687 798 1208 1348 1665 1827 3040 4500 6784)
FILENAME="./unit_test"

for i in ${lambda[@]}
do
    echo " ==== test p = $i ==== "
    echo " ==== test p = $i ==== " >> ${FILENAME}
    echo >> ${FILENAME}
    echo "TEST...$i"
    echo "TEST...$i" >> ${FILENAME}
    ./TRANS_Test $i>> ${FILENAME}
    echo >> ${FILENAME}
done

