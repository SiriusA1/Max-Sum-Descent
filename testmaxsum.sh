#!/bin/bash

file=maxsumdescent.cpp

if [ ! -f "$file" ]; then
    echo -e "Error: File '$file' not found.\nTest failed."
    exit 1
fi

MAXTIME="0.200"
num_right=0
total=0
line="________________________________________________________________________"
compiler=
interpreter=
language=
extension=${file##*.}
if [ "$extension" = "py" ]; then
    if [ ! -z "$PYTHON_PATH" ]; then
        interpreter=$(which python.exe)
    else
        interpreter=$(which python3.2)
    fi
    command="$interpreter $file"
    echo -e "Testing $file\n"
elif [ "$extension" = "java" ]; then
    language="java"
    command="java ${file%.java}"
    echo -n "Compiling $file..."
    javac $file
    echo -e "done\n"
elif [ "$extension" = "c" ] || [ "$extension" = "cpp" ]; then
    language="c"
    command="./${file%.*}"
    echo -n "Compiling $file..."
    results=$(make 2>&1)
    if [ $? -ne 0 ]; then
        echo -e "\n$results"
        exit 1
    fi
    echo -e "done\n"
fi

run_test_args() {
    (( ++total ))
    echo -n "Running test $total..."
    expected=$2
    local ismac=0
    date --version >/dev/null 2>&1
    if [ $? -ne 0 ]; then
       ismac=1
    fi
    local start=0
    if (( ismac )); then
        start=$(python -c 'import time; print time.time()')
    else
        start=$(date +%s.%N)
    fi
    received=$($command $1 2>&1 | tr -d '\r')
    local end=$(date +%s.%N)
    if (( ismac )); then
        end=$(python -c 'import time; print time.time()')
    else
        end=$(date +%s.%N)
    fi
    local elapsed=$(echo "scale=3; $end - $start" | bc | awk '{printf "%.3f", $0}') 
    if [ "$expected" != "$received" ]; then
        echo -e "failure\n\nExpected$line\n$expected\n"
        echo -e "Received$line\n$received\n"
    else
        result=$(echo $elapsed $MAXTIME | awk '{if ($1 > $2) print 1; else print 0}')
        if [ "$result" -eq 1 ]; then
            echo -e "failure\nTest timed out at $elapsed seconds. Maximum time allowed is $MAXTIME seconds.\n"
        else
            echo "success [$elapsed seconds]"
            (( ++num_right ))
        fi
    fi
}

(cat << ENDOFTEXT
75
95 64
ENDOFTEXT
) > items.txt
run_test_args "items.txt" "75"$'\n'"95 64"$'\n'"Max sum: 170"$'\n'"Values: [75, 95]"

rm -rf items.txt

echo -e "\nTotal tests run: $total"
echo -e "Number correct : $num_right"
echo -n "Percent correct: "
echo "scale=2; 100 * $num_right / $total" | bc

if [ "$language" = "java" ]; then
    echo -e -n "\nRemoving class files..."
    rm -f *.class
    echo "done"
elif [ $language = "c" ]; then
    echo -e -n "\nCleaning project..."
    make clean > /dev/null 2>&1
    echo "done"
fi
