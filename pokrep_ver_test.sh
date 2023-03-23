#!/bin/zsh

make clean_all
make all


for i in {1..100}
do
    echo "setup"
    ./TRANS_Setup 2048 10
    echo "commit"
    ./TRANS_Commit
    echo "open"
    ./TRANS_Open
    echo "verify"
    ./TRANS_Verify
done
    