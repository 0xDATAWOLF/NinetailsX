add_library(nxcore INTERFACE "./primitives.h" "./helpers.h")

add_library(NinetailsXEngine SHARED "./engine.h" "./engine.cpp")
target_link_libraries(NinetailsXEngine PUBLIC nxcore)