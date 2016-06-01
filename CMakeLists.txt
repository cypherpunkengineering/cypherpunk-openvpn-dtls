cmake_minimum_required(VERSION 3.0)

include(CheckIncludeFiles)

include(CheckCXXCompilerFlag)

enable_language(C)

check_include_files(pthread.h HAVE_PTHREAD_H)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/src/config.h
	)

#
#
#

set(CYPHER_INCLUDE_SYMLINK_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CYPHER_INCLUDE_SYMLINK_TARGET ${CMAKE_CURRENT_BINARY_DIR}/cypher)

add_custom_command(
	OUTPUT ${CYPHER_INCLUDE_SYMLINK_TARGET}
	COMMAND ln -sf ${CYPHER_INCLUDE_SYMLINK_SOURCE} ${CYPHER_INCLUDE_SYMLINK_TARGET}
	COMMENT "Creating symlink ${CYPHER_INCLUDE_SYMLINK_TARGET} -> ${CYPHER_INCLUDE_SYMLINK_SOURCE} ..."
	)

add_custom_target(build_environment
	DEPENDS ${CYPHER_INCLUDE_SYMLINK_TARGET}
	)

#
#
#
project(dtls_multichannel)

file(GLOB dm_SOURCES src/*.cc)
list(SORT dm_SOURCES)

add_executable(dtls_multichannel ${dm_SOURCES})

add_dependencies(dtls_multichannel build_environment)

#
#
#
include_directories(${CMAKE_CURRENT_BINARY_DIR})

check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)

if(NOT COMPILER_SUPPORTS_CXX11)
	message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set_property(TARGET dtls_multichannel APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++11 ")

set_property(TARGET dtls_multichannel APPEND_STRING PROPERTY COMPILE_FLAGS "-g -O2 ")

set_property(TARGET dtls_multichannel APPEND_STRING PROPERTY LINK_FLAGS "-lboost_system ")
set_property(TARGET dtls_multichannel APPEND_STRING PROPERTY LINK_FLAGS "-lpthread ")
