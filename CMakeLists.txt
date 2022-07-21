cmake_minimum_required(VERSION 3.18)
project(logger C)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
set(CMAKE_C_STANDARD 11)

# - Try to find LIBTAR
#  LIBTAR_FOUND               True if libuuid got found
#  LIBTAR_INCLUDE_DIRS        Location of libuuid headers
#  LIBTAR_LIBRARIES           List of libraries to use libuuid

INCLUDE( FindPkgConfig )

IF ( LIBTAR_FIND_REQUIRED )
    SET( _pkgconfig_REQUIRED "REQUIRED" )
ELSE ( LIBTAR_FIND_REQUIRED )
    SET( _pkgconfig_REQUIRED "" )
ENDIF ( LIBTAR_FIND_REQUIRED )

IF ( LIBTAR_MIN_VERSION )
    PKG_SEARCH_MODULE( LIBTAR ${_pkgconfig_REQUIRED} libtar>=${LIBTAR_MIN_VERSION} )
ELSE ( LIBTAR_MIN_VERSION )
    PKG_SEARCH_MODULE( LIBTAR ${_pkgconfig_REQUIRED} libtar )
ENDIF ( LIBTAR_MIN_VERSION )

IF( NOT LIBTAR_FOUND AND NOT PKG_CONFIG_FOUND )
    FIND_PATH( LIBTAR_INCLUDE_DIRS libtar.h )
    FIND_LIBRARY( LIBTAR_LIBRARIES libtar)

    # Report results
    IF ( LIBTAR_LIBRARIES AND LIBTAR_INCLUDE_DIRS )
        SET( LIBTAR_FOUND 1 )
        IF ( NOT LIBTAR_FIND_QUIETLY )
            MESSAGE( STATUS "Found libtar: ${LIBTAR_LIBRARIES}" )
        ENDIF ( NOT LIBTAR_FIND_QUIETLY )
    ELSE ( LIBTAR_LIBRARIES AND LIBTAR_INCLUDE_DIRS )
        IF ( LIBTAR_FIND_REQUIRED )
            MESSAGE( SEND_ERROR "Could NOT find libtar" )
        ELSE ( LIBTAR_FIND_REQUIRED )
            IF ( NOT LIBTAR_FIND_QUIETLY )
                MESSAGE( STATUS "Could NOT find libtar" )
            ENDIF ( NOT LIBTAR_FIND_QUIETLY )
        ENDIF ( LIBTAR_FIND_REQUIRED )
    ENDIF ( LIBTAR_LIBRARIES AND LIBTAR_INCLUDE_DIRS )
ENDIF( NOT LIBTAR_FOUND AND NOT PKG_CONFIG_FOUND )

MARK_AS_ADVANCED( LIBTAR_LIBRARIES LIBTAR_INCLUDE_DIRS )

include_directories(include ${LIBTAR_INCLUDE_DIRS})

add_library(logger SHARED src/logger.c)

add_executable(LoggerTest test/main.c src/logger.c)
target_link_libraries(LoggerTest PRIVATE Threads::Threads ${LIBTAR_LIBRARIES})
