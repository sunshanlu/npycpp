cmake_minimum_required(VERSION 3.10)

project(npycpp LANGUAGES CXX VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTING "build test or not" ON)
option(BUILD_EXAMPLES "build examples or not" ON)

find_package(ZLIB REQUIRED)
find_package(spdlog REQUIRED)
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${PROJECT_NAME} SHARED src/NpzWriter.cc src/NpzReader.cc src/NpData.cc)
target_include_directories(
        ${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include}>
        ${ZLIB_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(
        ${PROJECT_NAME} PUBLIC
        ${ZLIB_LIBRARIES}
        spdlog::spdlog
        ${OpenCV_LIBS}
        Eigen3::Eigen
)

if (BUILD_TESTING)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif ()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()

# 安装配置
set(PACKAGE_INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/include)
set(PACKAGE_LIBS_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib)

# 添加安装指令
install(
        TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION ${PACKAGE_LIBS_INSTALL_DIR}
        ARCHIVE DESTINATION ${PACKAGE_LIBS_INSTALL_DIR}
)
install(
        DIRECTORY ${PROJECT_SOURCE_DIR}/include/npycpp
        DESTINATION ${PACKAGE_INCLUDE_INSTALL_DIR}
)
install(
        EXPORT ${PROJECT_NAME}Targets
        FILE ${PROJECT_NAME}Targets.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

configure_file(
        Config.cmake.in
        "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" @ONLY
)

install(
        FILES
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
        DESTINATION
        ${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}
)

# 输出配置信息，以查看是否配置正确
message(====================================================================)
message(STATUS "${PROJECT_NAME} Configure: ")
message(STATUS "CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_EXAMPLES:       ${BUILD_EXAMPLES}")
message(STATUS "BUILD_TESTING:        ${BUILD_TESTING}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(====================================================================)
