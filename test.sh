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

# try 0 0
# try 42 42
# try 21 '5+20-4'
# try 41 ' 12 + 34 - 5 '
# try 26 ' 2 * 3 + 4 * 5 '
# try 12 ' 3 * 20 / 5 '
# try 3 ' 1 + 4 / 2 '
# try 9 ' 3 * ( 1 + 2 ) '
try 1 ' 3 / 1 / 3; '
try 9 ' -3 * -3; '
try 3 ' -9 / -3; '
try 3 ' a = 3; + a; '
try 3 ' a = 7; - a + 10; '
try 1 ' 2 == 2; '
try 5 ' a = 3; 2 + a; '
try 2 ' a = b = 1; a + b; '
try 3 ' return 3; return 1; '
try 5 ' a = 4; return a + 1; '

echo OK

exit 0
