list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

project(example1)

set(SOURCES example1.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include/)

find_package(Boost REQUIRED COMPONENTS system)
target_link_libraries(${PROJECT_NAME} Boost::system)

find_package(URIParserpp REQUIRED)
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${URIParserpp_INCLUDE_DIR}")
add_dependencies(${PROJECT_NAME} URIParserpp)