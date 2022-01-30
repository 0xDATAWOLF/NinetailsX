add_library(nxcore INTERFACE "./primitives.h" "./helpers.h")

add_library(NinetailsXEngine SHARED "./engine/engine.h" "./engine/engine.cpp")
target_link_libraries(NinetailsXEngine PUBLIC nxcore)