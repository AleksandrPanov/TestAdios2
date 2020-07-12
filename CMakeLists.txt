cmake_minimum_required(VERSION 2.8) #Проверка версии CMake
project(Test_ADIOS2)			            #Название проекта
set(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

set(ADIOS2_DIR ${CMAKE_SOURCE_DIR}/../ADIOS2/build)
find_package(ADIOS2 REQUIRED)
add_subdirectory(PrintTest)