#!/usr/bin/env bats

@test "basic find" {
  result="$(diff <(./../myfind) <(find))"
  [ "$result" == "" ]
}

@test "addition using dc" {
  result="$(echo 2 2+p | dc)"
  [ "$result" -eq 4 ]
}