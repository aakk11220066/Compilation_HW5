cmake_minimum_required(VERSION 3.13)
project(HW5)

set(CMAKE_CXX_STANDARD 11)

add_executable(HW5 Framework.cpp Symbol.cpp lex.yy.cpp parser.tab.cpp Exceptions.cpp parser.ypp)