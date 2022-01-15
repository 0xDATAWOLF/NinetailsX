find_package(X11 REQUIRED)
link_libraries(${X11_LIBRARIES})
include_directories(${X11_INCLUDE_DIR})

add_executable(NinetailsX "./main.cpp")
target_link_libraries(NinetailsX PUBLIC nxcore)