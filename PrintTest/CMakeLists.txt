include_directories(include)
add_executable(PrintTest src/print.cpp include/print.h)
include_directories(${ADIOS2_INCLUDE_DIRS})
target_link_libraries(PrintTest ${ADIOS2_LIBRARIES})