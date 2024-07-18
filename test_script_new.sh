#!/bin/zsh

lambda=(687 798 1208 1348 1665 1827 3040 4500 6784)

for k in ${lambda[@]}
do
    FILENAME="./${k}_result"
    echo "" > ${FILENAME}
    echo "================ compile w/ optimization option -O2 ================" >> ${FILENAME}
    for i in {7..14}
    do
        mkdir -p previous_/${k}/$i
        cp previous_/${k}/${i}/Txt/pp.txt previous_/${k}/${i}/Txt/poly.txt Txt/
        echo "================ test n = $i ================"
        echo "================ test n = $i ================" >> ${FILENAME}
        echo "OPEN...."
        echo "OPEN...." >>  ${FILENAME}
        ./TRANS_Open >> ${FILENAME}
        echo >>  ${FILENAME}
        echo "VERIFY...."
        echo "VERIFY...." >>  ${FILENAME}
        ./TRANS_Verify >> ${FILENAME}
        echo >>  ${FILENAME}
        cp -r Txt previous_/${k}/$i/
        rm -rf Txt/*
    done

    mv record_pp.txt previous_/${k}/

done

: << "END"
687 /1208 /1665
798 / 1348 / 1827
3040 / 4500 / 5000 / 6784
FILENAME="./687__test_for_pp"
    cp -r Txt previous_/687/$i/
done
FILENAME="./5000_10_test_result.txt"

for i in {10..14}
do
    echo " ==== test n = $i ==== "
    echo " ==== test n = $i ==== " >> ${FILENAME}
    echo "Setup..."
    echo "Setup..." >> ${FILENAME}
    ./TRANS_Setup 5000 $i >> ${FILENAME}
    echo >> ${FILENAME}
    echo "TEST..."
    echo "TEST..." >> ${FILENAME}
    ./TRANS_Test >> ${FILENAME}
    echo >> ${FILENAME}
done
END
