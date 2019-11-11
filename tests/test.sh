#!/bin/sh

function assert {
  # First parameter is the message in case the assertion is not verified
  message="$1"
  shift

  received="$2"
  expected="$4"

  "$@"

  # Get the return code
  rc=$?

  # If everything is okay, there's nothing left to do
  [ $rc -eq 0 ] && echo "Success" && return 0

  # An error occured, retrieved the line and the name of the script where it happend
  set $(caller)

  # Get the date and time at which the assertion occured
  date=$(date "+%Y-%m-%d %T%z")

  # Output an error message on the standard error
  # Format: date script [pid]: message (linenumber, return code)
  echo "$date $2 [$$]: $message (line=$1, rc=$rc)" >&2
  echo "Expected $expected and received $received"
  # Exit with the return code of the assertion test
  exit $rc
}

assert "./myfind ~ and find ~ do not return the same number of lines" test $(./../myfind ~ | wc -l) -eq $(find ~ | wc -l)
assert "./myfind -L ~ and find -L ~ do not return the same number of lines" test $(./../myfind -L ~ | wc -l) -eq $(find -L ~ | wc -l)
