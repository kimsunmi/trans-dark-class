# Source Code: Efficient Transparent Polynomial Commitments for zk-SNARKs
- This is source code for Class group.

# Requirments
- gmp, fmp, antic, openssl
## MAC
- Check the library location. Set a soft link to the library on a MAC.

# Start
```
$ make all
$ ./TRANS_Setup ${parameter bit} ${$$log(degree)=\mu$$}
$ ./TRANS_Open
$ ./TRANS_Verify 
```
or The previously run parameters are stored in the 'previous_' folder, so execute with these parameters for all degrees from 7 to 14.
```
$ make all
$ ./test_script_new
```
- check Unit Operation. It required parameters.
```
$ ./unit_test
```
