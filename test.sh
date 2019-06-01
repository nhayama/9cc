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
try 3 ' foo = 3; + foo; '
try 3 ' bar = 7; - bar + 10; '
try 1 ' 2 == 2; '
try 1 ' 1 != 2; '
try 1 ' 3 <= 10; '
try 0 ' 3 < 3; '
try 2 ' foo = bar = 1; foo + bar; '
try 3 ' return 3; return 1; '
try 5 ' return1234 = 4; return return1234 + 1; '

echo OK

exit 0
