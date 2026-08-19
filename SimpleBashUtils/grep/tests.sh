#!/bin/bash

SUCCESS=0
FAIL=0
TOTAL=0
COUNTER=0

# ---------- COLORS ----------
GREEN="\033[32m"
RED="\033[31m"
YELLOW="\033[33m"
RESET="\033[0m"

# ---------- FILES ----------
tests=(
    "file1.txt"
    "file2.txt"
    "pattern.txt"
)

# ---------- FLAGS ----------
flags=(
    "-i"
    "-v"
    "-c"
    "-l"
    "-n"
    "-h"
    "-s"
    "-o"
    "-f pattern.txt"
)

flag_e=("-e Moscow -e today")
patterns=("Moscow" "today")

GREP=./s21_grep

# ---------- FUNCTION: RUN SINGLE TEST ----------
run_test() {
    local cmd="$1"
    ((TOTAL++))
    ((COUNTER++))

    # обычное сравнение твоего grep vs системный
    $GREP $cmd > s21.txt
    grep  $cmd > sys.txt

    if diff -q s21.txt sys.txt >/dev/null; then
        ((SUCCESS++))
        echo -e "${GREEN}${COUNTER} [SUCCESS]${RESET} $cmd"
    else
        ((FAIL++))
        echo -e "${RED}${COUNTER} [FAIL]${RESET} $cmd"
    fi

    rm -f s21.txt sys.txt
}

# ---------- TITLE ----------
title() {
    echo ""
    echo -e "${YELLOW}===== $1 =====${RESET}"
    echo ""
}

# =====================================================
#                    TEST BLOCKS
# =====================================================

title "TEST: -e FLAG"
for e in "${flag_e[@]}"; do
    for file in "${tests[@]}"; do
        run_test "$e $file"
    done
done

title "0 OPTION — 1 PATTERN — 1 FILE"
for p in "${patterns[@]}"; do
    for f in "${tests[@]}"; do
        run_test "$p $f"
    done
done

title "1 OPTION — 1 PATTERN — 1 FILE"
for fl in "${flags[@]}"; do
    for p in "${patterns[@]}"; do
        for f in "${tests[@]}"; do
            run_test "$fl $p $f"
        done
    done
done

title "2 OPTIONS — 1 PATTERN — 1 FILE"
for f1 in "${flags[@]}"; do
    for f2 in "${flags[@]}"; do
        [[ "$f1" == "$f2" ]] && continue
        for p in "${patterns[@]}"; do
            for file in "${tests[@]}"; do
                run_test "$f1 $f2 $p $file"
            done
        done
    done
done

title "1 OPTION — 1 PATTERN — 2 FILES"
for fl in "${flags[@]}"; do
    for p in "${patterns[@]}"; do
        for f1 in "${tests[@]}"; do
            for f2 in "${tests[@]}"; do
                [[ "$f1" == "$f2" ]] && continue
                run_test "$fl $p $f1 $f2"
            done
        done
    done
done

# ---------- SUMMARY ----------
echo ""
echo -e "${GREEN}SUCCESS: $SUCCESS${RESET}"
echo -e "${RED}FAIL: $FAIL${RESET}"
echo -e "${YELLOW}TOTAL: $TOTAL${RESET}"
