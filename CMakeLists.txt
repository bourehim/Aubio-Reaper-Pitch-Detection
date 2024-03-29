# Support CMake version shipping with Ubuntu 18.04 LTS
cmake_minimum_required(VERSION 3.10)

project(reaper_minimal_plugin VERSION 0.1.0 LANGUAGES CXX)

set(SOURCES
    src/main.cpp
    src/jackio.c
    src/utils.c
    )

# If we are not on Windows, we need to use SWELL
if (NOT WIN32)
    list(APPEND SOURCES
        lib/WDL/WDL/swell/swell-modstub-generic.cpp
    )
endif()

add_library(reaper_minimal_plugin SHARED
    ${SOURCES}
    )
target_include_directories(reaper_minimal_plugin
    PRIVATE
    lib/reaper
    lib/WDL
    /usr/include/aubio/  # Add Aubio include directory
    )
target_link_libraries(reaper_minimal_plugin
    PRIVATE
    aubio  # Link against Aubio library
    )

# If we use SWELL, we want to use the SWELL which REAPER provides itself
if (NOT WIN32)
    target_compile_definitions(reaper_minimal_plugin PRIVATE SWELL_PROVIDED_BY_APP)
endif()

# REAPER loads plugins only if they start with reaper_, not libreaper_ (so we need to strip "lib" prefix)
SET_TARGET_PROPERTIES(reaper_minimal_plugin PROPERTIES PREFIX "")

# We want strict C++-11
target_compile_features(reaper_minimal_plugin PRIVATE cxx_std_11)
set_target_properties(reaper_minimal_plugin PROPERTIES CXX_EXTENSIONS OFF)


# Specify the destination path
set(COPY_DESTINATION "/home/android1/Music/_REA_2/REAPER/UserPlugins/")

# Copy the executable to the destination path after it is compiled
add_custom_command(
    TARGET reaper_minimal_plugin
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:reaper_minimal_plugin>
            ${COPY_DESTINATION}
)

# Create a custom target to ensure the copy is executed after building the main target
add_custom_target(copy_target ALL
    DEPENDS reaper_minimal_plugin
)

# Make sure the copy_target is built after the main target
add_dependencies(copy_target reaper_minimal_plugin)
