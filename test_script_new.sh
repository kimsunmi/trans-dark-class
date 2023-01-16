#!/bin/zsh

echo "" > ./result_230116_2
make clean_all
make all

for i in {10..16}
do
    echo "================ test n = $i ================"
    echo "================ test n = $i ================" >> ./result_230116
    ./TRANS_Setup 2048 $i >> ./result_230116
    echo >>  ./result_230116
    ./TRANS_Commit >> ./result_230116
    echo >>  ./result_230116
    ./TRANS_Open >> ./result_230116
    echo >>  ./result_230116
    ./TRANS_Verify >> ./result_230116
    echo >>  ./result_230116
done