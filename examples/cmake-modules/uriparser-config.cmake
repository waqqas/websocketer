include(ExternalProject)

if(WIN32)
    if(NOT TARGET URIParser_project)
        ExternalProject_Add(URIParser_project 
            URL https://github.com/uriparser/uriparser/releases/download/uriparser-0.9.4/uriparser-0.9.4.tar.gz
            CMAKE_ARGS 
                -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
                -DCMAKE_BUILD_TYPE:STRING=Release
                -DURIPARSER_BUILD_TESTS:BOOL=OFF
                -DURIPARSER_BUILD_DOCS:BOOL=OFF
                -DBUILD_SHARED_LIBS:BOOL=ON
        )
    endif()
else()
    if(NOT TARGET URIParser_project)
        ExternalProject_Add(URIParser_project 
            URL https://github.com/uriparser/uriparser/releases/download/uriparser-0.9.4/uriparser-0.9.4.tar.gz
            CMAKE_ARGS 
                -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
                -DCMAKE_BUILD_TYPE:STRING=Release
                -DURIPARSER_BUILD_TESTS:BOOL=OFF
                -DURIPARSER_BUILD_DOCS:BOOL=OFF
                -DBUILD_SHARED_LIBS:BOOL=OFF
        )
    endif()
endif()
set(URIParser_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include) 
set(URIParser_LIBS_DIR ${CMAKE_INSTALL_PREFIX}/lib) 

add_library(URIParser STATIC IMPORTED)

add_dependencies(URIParser URIParser_project)
