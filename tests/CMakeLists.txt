project(tests)

include(ExternalProject)
ExternalProject_Add(catch2 
    URL https://github.com/catchorg/Catch2/releases/download/v2.13.4/catch.hpp
    DOWNLOAD_DIR ${CMAKE_INSTALL_PREFIX}/include/catch2
    DOWNLOAD_NO_EXTRACT 1
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)

include(CodeCoverage)
append_coverage_compiler_flags()
if(APPLE)
  execute_process(COMMAND xcrun --sdk macosx --show-sdk-path OUTPUT_VARIABLE MACOSX_SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(COVERAGE_EXCLUDES "${MACOSX_SDK_PATH}/*" "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/include/c++/*")
endif()

setup_target_for_coverage_lcov(NAME coverage EXECUTABLE ${PROJECT_NAME} EXCLUDE "${CMAKE_INSTALL_PREFIX}/include/*" "/usr/local/include/*" NO_DEMANGLE)

file(GLOB SOURCES *.cpp *.h)
add_executable(${PROJECT_NAME} ${SOURCES})

add_dependencies(${PROJECT_NAME} catch2)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_INSTALL_PREFIX}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(${PROJECT_NAME} Boost::system)

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

find_package(OpenSSL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenSSL::SSL OpenSSL::Crypto)
