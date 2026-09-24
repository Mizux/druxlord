set(CURSES_NEED_NCURSES TRUE)
set(CURSES_NEED_WIDE TRUE)
find_package(Curses)

if(NOT CURSES_FOUND)
  message(STATUS "Curses not found, skipping druxlord-tui build")
  return()
endif()

add_executable(druxlord-tui)
target_sources(druxlord-tui PRIVATE
  main.cpp
  tui.cpp
  tui.h
)
target_include_directories(druxlord-tui PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  ${CURSES_INCLUDE_DIRS}
)
target_compile_definitions(druxlord-tui PRIVATE
  _DEFAULT_SOURCE
  _XOPEN_SOURCE=600
)
target_compile_features(druxlord-tui PRIVATE cxx_std_20)
target_compile_options(druxlord-tui PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -fstack-protector>
)
set_target_properties(druxlord-tui PROPERTIES
  VERSION ${PROJECT_VERSION}
  POSITION_INDEPENDENT_CODE ON
)
if(APPLE)
  set_target_properties(druxlord-tui PROPERTIES
    INSTALL_RPATH "@loader_path/../${CMAKE_INSTALL_LIBDIR}")
elseif(UNIX AND NOT APPLE)
  set_target_properties(druxlord-tui PROPERTIES
    INSTALL_RPATH "$ORIGIN/../${CMAKE_INSTALL_LIBDIR}")
endif()

target_link_libraries(druxlord-tui PRIVATE
  ${PROJECT_NAMESPACE}::libdruxlord
  ${CURSES_LIBRARIES}
)
add_executable(${PROJECT_NAMESPACE}::druxlord-tui ALIAS druxlord-tui)

# Install
install(TARGETS druxlord-tui
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
