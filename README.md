# Source Code: 

# Efficient Transparent Polynomial Commitments for zk-SNARKs
- This is source code for RSA group.

# Requirments
- gmp, fmp, antic, openssl
## MAC
- Check the library location. Set a soft link to the library on a MAC.

# Start
```
$ make all
$ ./TRANS_Setup ${parameter bit} ${$$log(degree)=\mu$$}
$ ./TRANS_Commit
$ ./TRANS_Open
$ ./TRANS_Verify 
```
or execute parameter bit in all degree 7~14
```
$ make all
$ ./test_script_new
```
