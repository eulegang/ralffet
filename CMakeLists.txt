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
  src/cmd.c
  src/embedded.c
)

install(TARGETS ralffet RUNTIME DESTINATION bin)
install(FILES docs/ralffet.1 DESTINATION share/man/man1)
