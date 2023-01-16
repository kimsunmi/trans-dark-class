# CC=gcc-12
CC=clang
CFLAGS := -g -lm -lcrypto -lgmp -lflint -fopenmp -lpthread -fsanitize=address

SOURCE_DIR = ./source/
TEST_CODE_DIR = ./test_code/

TARGET_TEST = TRANS_Test
TARGET_SETUP = TRANS_Setup
TARGET_COMMIT = TRANS_Commit
TARGET_OPEN = TRANS_Open
TARGET_VERIFY = TRANS_Verify
TARGET_precom = TRANS_precom

all:  $(TARGET_precom) $(TARGET_SETUP) $(TARGET_COMMIT) $(TARGET_OPEN) $(TARGET_VERIFY)
# $(TARGET_TEST)


clean:
	rm -rf *.dSYM *.a *.o $(TARGET_TEST)  $(TARGET_precom) $(TARGET_SETUP) $(TARGET_COMMIT) $(TARGET_OPEN) $(TARGET_VERIFY) 

clean_all:
	rm -rf *.dSYM *.a *.o $(TARGET_TEST) Txt/commit.txt Txt/pp.txt Txt/proof.txt Txt/poly.txt $(TARGET_SETUP) $(TARGET_COMMIT) $(TARGET_OPEN) $(TARGET_VERIFY)

$(TARGET_TEST): $(SOURCE_DIR)test.c 
	$(CC) -o $@ $(SOURCE_DIR)test.c $(SOURCE_DIR)Reducible_commitment.c $(SOURCE_DIR)Reducible_polynomial_commitment.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS)

$(TARGET_SETUP): $(TEST_CODE_DIR)setup_test.c
	$(CC) -o $@ $(TEST_CODE_DIR)setup_test.c $(SOURCE_DIR)Reducible_commitment.c $(SOURCE_DIR)Reducible_polynomial_commitment.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS) 

$(TARGET_COMMIT): $(TEST_CODE_DIR)commit_test.c
	$(CC) -o $@ $(TEST_CODE_DIR)commit_test.c $(SOURCE_DIR)Reducible_commitment.c $(SOURCE_DIR)Reducible_polynomial_commitment.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS) 

$(TARGET_OPEN): $(TEST_CODE_DIR)open_test.c
	$(CC) -o $@ $(TEST_CODE_DIR)open_test.c $(SOURCE_DIR)Reducible_commitment.c $(SOURCE_DIR)Reducible_polynomial_commitment.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS) 

$(TARGET_VERIFY): $(TEST_CODE_DIR)verify_test.c
	$(CC) -o $@ $(TEST_CODE_DIR)verify_test.c $(SOURCE_DIR)Reducible_commitment.c $(SOURCE_DIR)Reducible_polynomial_commitment.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS) 

$(TARGET_precom): $(SOURCE_DIR)precompute.c
	$(CC) -o $@ $(SOURCE_DIR)precompute.c  $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS)
