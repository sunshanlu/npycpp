add_definitions(-DEXAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(npycpp_examples NpycppExamples.cc)
target_link_libraries(npycpp_examples ${PROJECT_NAME})