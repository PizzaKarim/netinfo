# CMakeList.txt : CMake project for netinfo, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (netinfo "main.c" "network.c" "network.h" )
if (NOT WIN32)
  target_link_libraries(netinfo m)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET netinfo PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
