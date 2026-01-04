find_package(GTest REQUIRED)

add_executable(npdata_test NpDataTest.cc)
target_link_libraries(npdata_test
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

add_executable(npwriter_test NpzWriterTest.cc)
target_link_libraries(npwriter_test
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

add_executable(npreader_test NpzReaderTest.cc)
target_link_libraries(npreader_test
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

add_test(NAME npdata_test COMMAND npdata_test)
add_test(NAME npwriter_test COMMAND npwriter_test)
add_test(NAME npreader_test COMMAND npreader_test)