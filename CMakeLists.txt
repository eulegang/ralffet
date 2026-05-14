cmake_minimum_required(VERSION 3.10)
project(ralffet
  LANGUAGES C
  VERSION 0.1.0
  DESCRIPTION "Ralph looping in a unix style"
)
add_executable(
  ralffet

  src/main.c
  src/args.c
)

