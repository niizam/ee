cmake_minimum_required(VERSION 3.16)
project(ee LANGUAGES C)

option(EE_USE_INTERNAL_CURSES "Build with the bundled 'new_curse' implementation" OFF)

add_executable(ee ee.c)
target_include_directories(ee PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_property(TARGET ee PROPERTY C_STANDARD 99)
set_property(TARGET ee PROPERTY C_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

if(EE_USE_INTERNAL_CURSES)
  if(WIN32)
    message(FATAL_ERROR "The bundled curses implementation is not supported on Windows. Install PDCurses/ncurses or disable EE_USE_INTERNAL_CURSES.")
  endif()
  target_sources(ee PRIVATE new_curse.c)
  target_compile_definitions(ee PRIVATE NCURSE SYS5)
else()
  find_package(Curses REQUIRED)
  if(CURSES_INCLUDE_DIR)
    target_include_directories(ee PRIVATE ${CURSES_INCLUDE_DIR})
  endif()
  target_link_libraries(ee PRIVATE ${CURSES_LIBRARIES})
endif()

target_link_libraries(ee PRIVATE CURL::libcurl)

if(WIN32)
  target_compile_definitions(ee PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
