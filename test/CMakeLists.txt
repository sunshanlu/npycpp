find_package(GTest REQUIRED)

add_executable(npdata_test NpDataTest.cc)
target_link_libraries(npdata_test
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

add_executable(npzwriter_test NpzWriterTest.cc)
target_link_libraries(npzwriter_test
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

add_executable(npzreader_test NpzReaderTest.cc)
target_link_libraries(npzreader_test
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

add_executable(npyreader_test NpyReaderTest.cc)
target_link_libraries(npyreader_test
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)

add_executable(npywriter_test NpyWriterTest.cc)
target_link_libraries(npywriter_test
        ${PROJECT_NAME}
        GTest::gtest
        GTest::gtest_main
)


add_test(NAME npdata_test COMMAND npdata_test)
add_test(NAME npzwriter_test COMMAND npzwriter_test)
add_test(NAME npzreader_test COMMAND npzreader_test)
add_test(NAME npywriter_test COMMAND npywriter_test)
add_test(NAME npyreader_test COMMAND npyreader_test)
