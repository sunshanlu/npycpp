add_definitions(-DEXAMPLES_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(npzcpp_examples NpzcppExamples.cc)
target_link_libraries(npzcpp_examples ${PROJECT_NAME})

add_executable(npycpp_examples NpycppExamples.cc)
target_link_libraries(npycpp_examples ${PROJECT_NAME})