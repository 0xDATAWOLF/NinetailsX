add_library(nxcore INTERFACE "./primitives.h" "./helpers.h")

add_library(NinetailsXEngine SHARED "./engine/game.h" "./engine/game.cpp")
target_link_libraries(NinetailsXEngine PUBLIC nxcore)