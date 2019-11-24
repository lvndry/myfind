function check
{
  var="$*"
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
check src foo foo/hat
check src foo foo/hat -name bar
check src foo foo/hat -type d
check src foo foo/hat -type f
check ~/Desktop
check src/ -type d  \( -name baz -o -name bar \)
check foo -name bar -a -name baz
check foo -name bar -name baz
check foo/* -newer foo/baz
check src \! -name src
check '!' -name bar
check \( -name bar -o -name baz \) -type f -exec "echo" ";"
check \! \( -name bar -o -name baz \) -type d -print -o print
check \! \( -name bar -a -name baz \) -print
check -perm "644"
check  foo/ -user ouly
check foo/ -group ouly
check foo/ -group oulyy
check src -name src
check -H ~/Desktop
check -H qux
check -H
check -L .
check foo -name bar
check ./foo/// -name bar
check ./foo/// -name foo
check foo -name foo
check foo -name 'ba?'
check foo -name bar -o -name baz
check foo -name bar -a -name baz
check foo -name bar -name baz
check foo -newer foo/bar
check foo -newer foo/baz
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
check -print -print
check src -exec "pwd" ";"
check -exec "pwd" ";"
check src -exec "echo" "{}" ";"
check foo -exec "{}" ";"
check src -execdir "echo" "{}" ";"
check -execdir "echo" "{}" ";"
check foo -exec "pwd" ";" -exec "echo" "--" "{}" "--" ";"
check foo -exec "echo" "ok" ";"
check foo -execdir "echo" "--" "{}" "--" ";"
check -execdir "pwd" ";" -execdir "echo" "--" "{}" "--" ";"
check -perm 333 -a \! -print -a -name bar
check -perm 333 -a \! -print -o -name foo
check -perm 333 -a \! -print -a aswaf
check -perm 333 -a \! -print -a -o -name foo
check -perm 333 -a \! -print -o -o -name foo
check -perm 333 -a \! -print -o -a -name foo
check -perm 333 -a \! -print -o -a -a -o -name foo
check -perm 333 -a -not -print -o -name foo
check -perm 333 -a \( \! -print -o -name foo \)
check \( -name \)