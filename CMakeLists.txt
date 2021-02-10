cmake_minimum_required(VERSION 3.12)
project("websocketer" VERSION "0.1.0" DESCRIPTION "A header-only websocket client library build on Boost::Beast")

list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

include(GNUInstallDirs)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE
      STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set (CMAKE_CXX_FLAGS_DEBUG  "${CMAKE_CXX_FLAGS_DEBUG} \
  -fPIC -Wall -pedantic -Wextra -Werror \
  -Wno-missing-braces -Wno-unused-variable \
  -Wno-ignored-qualifiers  -fdiagnostics-color")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -DNDEBUG -O3")

add_library(${PROJECT_NAME} INTERFACE)

find_package(Certify REQUIRED)
target_include_directories(${PROJECT_NAME} INTERFACE "${Certify_INCLUDE_DIR}")
add_dependencies(${PROJECT_NAME} Certify)

target_include_directories(
  ${PROJECT_NAME}
  INTERFACE $<BUILD_INTERFACE:${${PROJECT_NAME}_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_17)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_Targets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_NAME}ConfigVersion.cmake"
                                    VERSION ${PROJECT_VERSION}
                                    COMPATIBILITY SameMajorVersion)
               
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(EXPORT ${PROJECT_NAME}_Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME} DESTINATION include)

option(WEBSOCKETER_BUILD_TESTS "Enable building tests" ON)
if(WEBSOCKETER_BUILD_TESTS)
    add_subdirectory(tests)
endif()

option(WEBSOCKETER_BUILD_BENCHMARK "Build benchmark" ON)
if(WEBSOCKETER_BUILD_BENCHMARK)
    add_subdirectory(benchmarks)
endif()

add_subdirectory(docker)

add_custom_target(test
  VERBATIM
  COMMAND ./tests/tests
)