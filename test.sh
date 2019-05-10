#!/bin/bash
try() {
    expected="$1"
    input="$2"

    ./9cc "$input" > tmp.s
    gcc -o tmp tmp.s
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
	echo "$input => $actual"
    else
	echo "$expected expected, but got $actual"
	exit 1
    fi
}

try 1 ' 3 / 1 / 3; '
try 9 ' -3 * -3; '
try 3 ' a = 3; + a; '
try 3 ' a = 7; - a + 10; '
try 1 ' 2 == 2; '
try 1 ' 1 != 2; '
try 1 ' 3 <= 10; '
try 0 ' 3 < 3; '
try 2 ' a = b = 1; a + b; '
try 3 ' return 3; return 1; '
try 5 ' a = 4; return a + 1; '

echo OK

exit 0
