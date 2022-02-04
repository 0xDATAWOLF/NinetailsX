

add_executable(NinetailsX WIN32 main.cpp)
target_link_libraries(NinetailsX PUBLIC nxcore winmm.lib)

add_custom_command(TARGET NinetailsX POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_SOURCE_DIR}/assets
                ${CMAKE_BINARY_DIR}/bin/Debug/assets)
