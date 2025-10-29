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

  # Prefer the include dirs provided by FindCurses if present
  if(CURSES_INCLUDE_DIRS)
    target_include_directories(ee PRIVATE ${CURSES_INCLUDE_DIRS})
  elseif(CURSES_INCLUDE_DIR)
    target_include_directories(ee PRIVATE ${CURSES_INCLUDE_DIR})
  endif()

  # On MSYS2/MinGW, headers are often under ncurses/ or ncursesw/.
  # If there is no top-level ncurses.h, add the subdir so #include <ncurses.h> works.
  set(_curses_inc_hint "")
  if(CURSES_INCLUDE_DIRS)
    list(GET CURSES_INCLUDE_DIRS 0 _curses_inc_hint)
  elseif(CURSES_INCLUDE_DIR)
    set(_curses_inc_hint "${CURSES_INCLUDE_DIR}")
  endif()
  if(NOT _curses_inc_hint)
    # Fallback guess for MSYS2 UCRT64
    set(_curses_inc_hint "/ucrt64/include")
  endif()

  if(EXISTS "${_curses_inc_hint}/ncursesw/ncurses.h")
    target_include_directories(ee PRIVATE "${_curses_inc_hint}/ncursesw")
    target_compile_definitions(ee PRIVATE HAS_NCURSES)
  elseif(EXISTS "${_curses_inc_hint}/ncurses/ncurses.h")
    target_include_directories(ee PRIVATE "${_curses_inc_hint}/ncurses")
    target_compile_definitions(ee PRIVATE HAS_NCURSES)
  elseif(EXISTS "${_curses_inc_hint}/ncurses.h")
    # Some distros provide a top-level ncurses.h
    target_compile_definitions(ee PRIVATE HAS_NCURSES)
  endif()

  target_link_libraries(ee PRIVATE ${CURSES_LIBRARIES})
endif()

target_link_libraries(ee PRIVATE CURL::libcurl)

if(WIN32)
  target_compile_definitions(ee PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
