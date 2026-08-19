#!/bin/bash
SUCCESS=0
FAIL=0
COUNTER=0

tests=(
    "file1.txt"
    "file2.txt"
    "file3.txt"
    "file4.txt"
)

CAT=./s21_cat
flags=(
    "-b"
    "-e"
    "-n"
    "-s"
    "-t"
    "-v"
)

printf "\n"
echo "================="
echo "1 OPTION - 1 FILE"
echo "================="
printf "\n"

for v1 in "${flags[@]}"
do
    for v2 in "${tests[@]}"
    do 
        ((COUNTER++))
        TEST="$v1 $v2"
        $CAT $TEST > test_s21_cat.txt
        cat $TEST > test_cat.txt
        if diff -q test_s21_cat.txt test_cat.txt >/dev/null; then
            ((SUCCESS++))
            echo "$COUNTER - [SUCCESS]  $TEST"
        else
            ((FAIL++))
            echo "$COUNTER - [FAIL] $TEST"
        fi
        rm test_s21_cat.txt test_cat.txt
    done
done

printf "\n"
echo "================="
echo "2 OPTION - 1 FILE"
echo "================="
printf "\n"

for v1 in "${flags[@]}"
do
    for v2 in "${flags[@]}"
    do
        if [ $v1 != $v2 ]; then
            for v3 in "${tests[@]}"
            do
                    ((COUNTER++))
                    TEST="$v1 $v2 $v3"
                    $CAT $TEST > test_s21_cat.txt
                    cat $TEST > test_cat.txt
                    if diff -q test_s21_cat.txt test_cat.txt >/dev/null; then
                        ((SUCCESS++))
                        echo "$COUNTER - [SUCCESS]  $TEST"
                    else
                        ((FAIL++))
                        echo "$COUNTER - [FAIL] $TEST"
                    fi
                    rm test_s21_cat.txt test_cat.txt
            done
        fi
    done
done

printf "\n"
echo "================="
echo "1 OPTION - 2 FILE"
echo "================="
printf "\n"

for v1 in "${flags[@]}"
do
  for v2 in "${tests[@]}"
    do
        for v3 in "${tests[@]}"
        do
            if [ $v3 != $v2 ]; then
                ((COUNTER++))
                TEST="$v1 $v2 $v3"
                $CAT $TEST > test_s21_cat.txt
                cat $TEST > test_cat.txt
                if diff -q test_s21_cat.txt test_cat.txt >/dev/null; then
                    ((SUCCESS++))
                    echo "$COUNTER - [SUCCESS]  $TEST"
                else
                    ((FAIL++))
                    echo "$COUNTER - [FAIL] $TEST"
                fi
                rm test_s21_cat.txt test_cat.txt
            fi
        done
    done
done

printf "\n"
echo "================="
echo "2 OPTION - 2 FILE"
echo "================="
printf "\n"

for v1 in "${flags[@]}"
do
    for v2 in "${flags[@]}"
    do
        if [ $v1 != $v2 ]; then
            for v3 in "${tests[@]}"
            do
                for v4 in "${tests[@]}"
                do
                    if [ $v3 != $v4 ]; then
                        ((COUNTER++))
                        TEST="$v1 $v2 $v3 $v4"
                        $CAT $TEST > test_s21_cat.txt
                        cat $TEST > test_cat.txt
                        if diff -q test_s21_cat.txt test_cat.txt >/dev/null; then
                            ((SUCCESS++))
                            echo "$COUNTER - [SUCCESS]  $TEST"
                        else
                            ((FAIL++))
                            echo "$COUNTER - [FAIL] $TEST"
                        fi
                        rm test_s21_cat.txt test_cat.txt
                    fi
                done
            done
        fi
    done
done

echo "SUCCESS: ${SUCCESS}"
echo "FAILED: ${FAIL}"
echo "TOTAL: ${COUNTER}"