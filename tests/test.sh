#!/bin/sh

function check
{
	var="$*"
  echo "Testing ./myfind $var vs find $var"
  ./myfind ${var} > a.log
	find ${var} > b.log
	DIFF=$(diff a.log b.log)
	if [ "$DIFF" != "" ]
	then
		printf "%b\n" "$var"
		printf "%s\n" $DIFF;
	else
		printf "Ok\n";
	fi
}

check src
check ~/Desktop
check src/ -type d  \( -name baz -o -name bar \)
check src \! -name src
check '!' -name bar
check \( -name bar -o -name baz \) -print
check \! \( -name bar -o -name baz \) -print
check -perm "644"
check  foo/ -user ouly
check foo/ -group ouly
check src -name src
check -H ~/Desktop
check -L .
check foo -name bar
check foo -name foo
check foo -name 'ba?'
check foo -name bar -o -name baz
check foo -name bar -a -name baz
check foo -name bar -name baz
check foo -newer foo/qux
check foo -newer foo/hat
check foo -name 'foo*' -print -name 'foobar' -print
check -L .
check -H qux
check ../ -type b
check ../ -type c
check ../ -type d
check ../ -type f
check ../ -type l
check ../ -type p
check ../ -type s
check -print -o -print
check -print -a -print
check src -exec "pwd" ";"
check src -exec "echo" "{}" ";"
check src -execdir "echo" "{}" ";"
check foo -exec "pwd" ";" -exec "echo" "--" "{}" "--" ";"
check foo -exec "echo" "ok" ";"
check foo -execdir "echo" "--" "{}" "--" ";"