include(ExternalProject)

if(NOT TARGET URIParserpp_project)
    ExternalProject_Add(URIParserpp_project 
        URL https://github.com/waqqas/uripaserpp/archive/v0.1.0.tar.gz
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
    )

    ExternalProject_Get_Property(URIParserpp_project SOURCE_DIR)

    ExternalProject_Add(URIParserpp_install_project 
        DOWNLOAD_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND cp -R ${SOURCE_DIR}/include ${CMAKE_INSTALL_PREFIX}
    )
endif()

ExternalProject_Get_Property(URIParserpp_project SOURCE_DIR)

set(URIParserpp_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include) 

add_library(URIParserpp STATIC IMPORTED)

add_dependencies(URIParserpp_install_project URIParserpp_project)
add_dependencies(URIParserpp URIParserpp_install_project URIParser)