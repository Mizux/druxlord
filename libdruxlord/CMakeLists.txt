add_library(libdruxlord SHARED)
target_sources(libdruxlord PRIVATE
  druxlord.cpp
  druxlord.h
)
target_include_directories(libdruxlord PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/druxlord>
)
target_compile_features(libdruxlord PUBLIC cxx_std_20)
target_compile_options(libdruxlord PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -fstack-protector>
)
set_target_properties(libdruxlord PROPERTIES
  PREFIX ""
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  POSITION_INDEPENDENT_CODE ON
  WINDOWS_EXPORT_ALL_SYMBOLS ON
  PUBLIC_HEADER druxlord.h
)
if(APPLE)
  set_target_properties(libdruxlord PROPERTIES
    INSTALL_RPATH "@loader_path")
elseif(UNIX AND NOT APPLE)
  set_target_properties(libdruxlord PROPERTIES
    INSTALL_RPATH "$ORIGIN")
endif()
add_library(${PROJECT_NAMESPACE}::libdruxlord ALIAS libdruxlord)

# Install
install(TARGETS libdruxlord
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/druxlord
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
