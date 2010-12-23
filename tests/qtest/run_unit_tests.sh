#!/bin/sh

PACKAGE=crash-reporter-tests
TEST_BIN_DIR=/usr/lib
TEST_LOG_FILE=/tmp/result.log

if [ -e $TEST_LOG_FILE ]; then
    rm -f $TEST_LOG_FILE
fi

echo "Starting to run unit tests..."

for TESTCASE in `ls $TEST_BIN_DIR/$PACKAGE/ut_*`; do
    echo "Running" $TESTCASE
    $TESTCASE -v2 >> $TEST_LOG_FILE 2>&1
done

echo "Test run finished. Log file written in:" $TEST_LOG_FILE

grep 'FAIL' $TEST_LOG_FILE 