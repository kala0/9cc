#!/bin/bash

try() {
	expected="$1"
	input="$2"

	./9cc "$input" > tmp.s
	gcc tmp.s -o tmp
	./tmp
	actual="$?"

	if [ "$actual" != "$expected" ]; then
		echo "$input expected, but got $actual"
		exit 1
	fi
}

try 0 0
try 42 42
try 21 '5+20-4'

echo OK
