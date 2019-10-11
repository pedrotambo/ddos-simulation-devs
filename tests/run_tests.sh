#!/bin/bash

if [ $1 == '-h' ]
then
    echo 'Usage: ./run_tests.sh [-h] <model> --Corre los casos de tests para el directorio en ./<model>'
    echo '       Por defecto corre los modelos definidos en ALL_TEST_CASES'
    exit
fi

ALL_TEST_CASES=('dispatcher dispatcher-server queue server scaler')
PARENT_PATH=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
BIN_PATH='../../bin/cd++'

V=$1
TEST_INPUT=${V:-"all"}

if [ $TEST_INPUT == 'all' ]
then
    TEST_CASES=$ALL_TEST_CASES
else 
    TEST_CASES=($1)
fi

for tc in ${TEST_CASES[@]}
do
    echo $tc
    cd $tc
    echo $(pwd)
    NUM_TESTS=$(ls | grep ma | wc -l)
    NUM_TESTS_FROM_ZERO=$(($NUM_TESTS - 1))
    mkdir -p 'logs'


    echo $NUM_TESTS_FROM_ZERO
    for ((i = 0; i <= NUM_TESTS_FROM_ZERO; i++));
    do
        if [ $i -le 9 ]
        then
            num="0${i}"
        else
            num=${i}
        fi
        echo;echo
        echo "==================================="
        echo "         RUNNING TEST ${i}         "
        echo "==================================="
        echo;echo
        mkdir -p "logs/test${num}"
        ./$BIN_PATH -m "test${num}.ma" -e "test${num}.ev" -t 00:01:00:000 -l "logs/test${num}/"
    done
    cd ..
    echo $(pwd)
done