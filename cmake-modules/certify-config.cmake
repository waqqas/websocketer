include(ExternalProject)

if(NOT TARGET Certify_project)
ExternalProject_Add(Certify_project 
GIT_REPOSITORY https://github.com/waqqas/certify.git
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)
endif()

ExternalProject_Get_Property(Certify_project SOURCE_DIR)

if(NOT TARGET Certify_install_project)
ExternalProject_Add(Certify_install_project 
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND cp -R ${SOURCE_DIR}/include ${CMAKE_INSTALL_PREFIX}
)
endif()

ExternalProject_Get_Property(Certify_project SOURCE_DIR)

set(Certify_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/include) 

if(NOT TARGET Certify)
add_library(Certify STATIC IMPORTED)
endif()

add_dependencies(Certify_install_project Certify_project)
add_dependencies(Certify Certify_install_project)