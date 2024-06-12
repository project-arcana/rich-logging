cmake_minimum_required(VERSION 3.8)
project(RichLogging)

if (NOT TARGET clean-core)
    message(FATAL_ERROR "[rich-log] clean-core must be available")
endif()

# =========================================
# global options

option(RLOG_FORCE_MACRO_PREFIX "if true, only RICH_ macro versions are available" OFF)

# =========================================
# define library

file(GLOB_RECURSE SOURCES "src/*.cc")
file(GLOB_RECURSE HEADERS "src/*.hh")

arcana_add_library(RLOG rich-log SOURCES HEADERS)

target_include_directories(rich-log PUBLIC src/)

target_link_libraries(rich-log PUBLIC
    clean-core
    reflector
)


# =========================================
# set up compile flags

# the second option is for backwards compatibility with the previous name
if (RLOG_FORCE_MACRO_PREFIX OR RICH_LOG_FORCE_MACRO_PREFIX)
    target_compile_definitions(rich-log PUBLIC RICH_LOG_FORCE_MACRO_PREFIX)
endif()
