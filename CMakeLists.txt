PROJECT(external-sort)
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)
SET(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS_DEBUG "-O3 -D_NDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -D_NDEBUG")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread -lpthread" CACHE INTERNAL "Linker options")

####################################################################
##           ASSEMBLE EXTERNAL SORT AS A SHARED LIBRARY           ##
####################################################################

SET(LIBRARY_NAME "ExternalSort")

include_directories(${CMAKE_SOURCE_DIR}/include/)
file(GLOB_RECURSE SOURCES_CPP_LIBRARY ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE SOURCES_C_LIBRARY ${CMAKE_SOURCE_DIR}/src/*.c)
file(GLOB_RECURSE HEADERS_LIBRARY ${CMAKE_SOURCE_DIR}/include/*.h)

list(APPEND SOURCES_LIBRARY ${SOURCES_C_LIBRARY})
list(APPEND SOURCES_LIBRARY ${SOURCES_CPP_LIBRARY})

add_library(${LIBRARY_NAME} SHARED ${SOURCES_LIBRARY})

set_target_properties(${LIBRARY_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

####################################################################
##                  ASSEMBLE LIBRARY EXAMPLES                     ##
####################################################################

SET(EXAMPLE_PROJECT ${LIBRARY_NAME}-example)
option(EXAMPLE_REQUIRED "Build library with examples" OFF)

if (EXAMPLE_REQUIRED)
    include_directories(${CMAKE_SOURCE_DIR}/examples/)
    file(GLOB_RECURSE SOURCES_CPP_EXAMPLE ${CMAKE_SOURCE_DIR}/examples/*.cpp)
    file(GLOB_RECURSE SOURCES_C_EXAMPLE ${CMAKE_SOURCE_DIR}/examples/*.c)
    file(GLOB_RECURSE SOURCES_HEADER_EXAMPLE ${CMAKE_SOURCE_DIR}/examples/*.h)

    list(APPEND SOURCES_EXAMPLE ${SOURCES_CPP_EXAMPLE})
    list(APPEND SOURCES_EXAMPLE ${SOURCES_C_EXAMPLE})

    add_executable(${EXAMPLE_PROJECT} ${SOURCES_EXAMPLE})
    set_target_properties(${EXAMPLE_PROJECT} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )
    target_link_libraries(${EXAMPLE_PROJECT} ${LIBRARY_NAME})
endif(EXAMPLE_REQUIRED)

####################################################################
##                        INSTALL LIBRARY                         ##
####################################################################

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include/external_sort)
install(TARGETS ${LIBRARY_NAME}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
