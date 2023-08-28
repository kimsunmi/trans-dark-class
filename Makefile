CC=gcc
# CC=clang
CFLAGS := -g -lm -lcrypto -lgmp -lflint -lantic -fopenmp -lpthread # -fsanitize=address
SOURCE_DIR = ./source/
TEST_CODE_DIR = ./test_code/

TARGET_TEST = TRANS_Test
TARGET_SETUP = TRANS_Setup
TARGET_OPEN = TRANS_Open
TARGET_VERIFY = TRANS_Verify
TARGET_precom = TRANS_precom

setup: $(TARGET_SETUP) $(TARGET_OPEN) $(TARGET_VERIFY)
open: $(TARGET_OPEN)
test: $(TARGET_SETUP) $(TARGET_TEST)

all:  $(TARGET_precom) $(TARGET_SETUP) $(TARGET_OPEN) $(TARGET_OPEN) $(TARGET_VERIFY)

clean:
	rm -rf *.dSYM *.a *.o $(TARGET_TEST)  $(TARGET_precom) $(TARGET_SETUP) $(TARGET_OPEN) $(TARGET_OPEN) $(TARGET_VERIFY) 

clean_all:
	rm -rf *.dSYM *.a *.o $(TARGET_TEST) Txt/commit.txt Txt/pp.txt Txt/proof.txt Txt/poly.txt $(TARGET_SETUP) $(TARGET_OPEN) $(TARGET_OPEN) $(TARGET_VERIFY)

$(TARGET_TEST): $(SOURCE_DIR)test.c 
	$(CC) -o $@ $(SOURCE_DIR)test.c $(SOURCE_DIR)polynomial_commit.c $(SOURCE_DIR)polynomial_open_verify.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS)

$(TARGET_SETUP): $(TEST_CODE_DIR)setup_test.c
	$(CC) -o $@ $(TEST_CODE_DIR)setup_test.c $(SOURCE_DIR)polynomial_commit.c $(SOURCE_DIR)polynomial_open_verify.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS) 

$(TARGET_OPEN): $(TEST_CODE_DIR)open_test.c
	$(CC) -o $@ $(TEST_CODE_DIR)open_test.c $(SOURCE_DIR)polynomial_commit.c $(SOURCE_DIR)polynomial_open_verify.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS) 

$(TARGET_VERIFY): $(TEST_CODE_DIR)verify_test.c
	$(CC) -o $@ $(TEST_CODE_DIR)verify_test.c $(SOURCE_DIR)polynomial_commit.c $(SOURCE_DIR)polynomial_open_verify.c $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS) 

$(TARGET_precom): $(SOURCE_DIR)precompute.c
	$(CC) -o $@ $(SOURCE_DIR)precompute.c  $(SOURCE_DIR)util.c $(SOURCE_DIR)codeTimer.c $(CFLAGS)
