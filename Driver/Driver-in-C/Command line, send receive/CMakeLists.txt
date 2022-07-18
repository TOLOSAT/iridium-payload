cmake_minimum_required(VERSION 3.16.3)

project(driver)

add_executable ( driver src/utility.c src/iridium.c src/main.c)

include_directories(include)
