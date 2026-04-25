cmake_minimum_required(VERSION 3.28 FATAL_ERROR)

# Project.
project(train_rail_simple_sim
    VERSION 0.1.0
    LANGUAGES CXX C)

# When turning this option on, the library will be compiled using assertions. By default asserts are enabled in Debug build.
option(ENABLE_PROFILING "Enable profiling" OFF)

# When turning this option on, the logging version of the FMOD library will be linked.
option(ENABLE_FMOD_LOGGING "Enable FMOD logging" OFF)

# Print out all options.
message(
    "\n"
    "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n"
    "|               ::OPTION VALUES::                \n"
    "| ENABLE_PROFILING    : ${ENABLE_PROFILING}\n"
    "| ENABLE_FMOD_LOGGING : ${ENABLE_FMOD_LOGGING}\n"
    "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n")

# App icon.
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    enable_language("RC")
    set(WIN64_RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/win64/app_icon.rc)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    message(WARNING "MACOS APP ICON STUFF: https://stackoverflow.com/questions/22745728/cmake-set-icon-for-a-mac-os-x-app")
else()
    message(FATAL_ERROR "Asset folder location must be defined in `ASSET_DIR`")
endif()

# C/C++ standard.
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # MacOS requires enabling experimental library for clang-17 (default).
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")
endif()

# Ensure 64-bit build.
if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Only 64-bit build allowed.")
    return()
endif()

# Multithreaded build.
set(CMAKE_BUILD_PARALLEL_LEVEL 16)
# if(MSVC)  # @NOTE: this can cause errors so don't do this lol.  -Thea 2026/02/14
#     set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
# endif()

# Profiling.
if(ENABLE_PROFILING)
    message(FATAL_ERROR "@TODO: Implement profiling libraries here!")
endif()

# Assets directory.
set(ASSET_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/assets)

# Check certain env vars exist before dependency list.
if(NOT DEFINED ENV{FMOD_STUDIO_API_DIR})
    message(FATAL_ERROR "Env var \"FMOD_STUDIO_API_DIR\" is not defined. Set it to the base dir of the FMOD API and retry.")
endif()

# Dependencies.
set(DEPENDENCY_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/cpp_glm/include  # Header only.
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/entt/src  # Header only.
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/fmt/include
    $ENV{FMOD_STUDIO_API_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui  # Header only.
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/JoltPhysics
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb  # Header only (essentially).
    # ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stduuid/include  # @NOTE: Removing this so that it's not easy to mix up the uuid.h header files.
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/tomlplusplus/include  # Header only.
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/TXP_renderer/include
)

add_subdirectory(third_party/fmt)
set(DOUBLE_PRECISION ON CACHE BOOL "Use double precision math" FORCE)  # Use double precision for large, open world physics.
add_subdirectory(third_party/JoltPhysics/Build)
add_subdirectory(third_party/json)
add_subdirectory(third_party/stduuid)
add_subdirectory(third_party/TXP_renderer)

# Create symlink to assets folder.
if(DEFINED ASSET_DIR)
    set(dir_name ${ASSET_DIR})
    set(target_dir_name ${CMAKE_BINARY_DIR}/assets)
    if(WIN32)
        string(REPLACE "/" "\\" win32_dir_name ${dir_name})
        string(REPLACE "/" "\\" win32_target_dir_name ${target_dir_name})
        if(EXISTS ${dir_name})
            execute_process(
                COMMAND cmd /C rmdir ${win32_target_dir_name}
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
        endif()
        execute_process(
            COMMAND cmd /C mklink /J ${win32_target_dir_name} ${win32_dir_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    else()
        # The way you create a symlink in every single OS except Windows.s
        file(CREATE_LINK ${dir_name} ${target_dir_name} SYMBOLIC)
    endif()
endif()


# Resource files inside ASSET_DIR.
set(ASSET_DIR_FILES

)

# macOS: add all files in assets dir into app bundle package.
if (TXP_RENDERER_STANDALONE)
    foreach (FILE ${ASSET_DIR_FILES})
        file(RELATIVE_PATH NEW_FILE "${ASSET_DIR}" ${FILE})
        get_filename_component(NEW_FILE_PATH ${NEW_FILE} DIRECTORY)
        set_source_files_properties(${FILE}
            PROPERTIES
                MACOSX_PACKAGE_LOCATION "Resources/assets/${NEW_FILE_PATH}"
        )
    endforeach()
endif()


# Main source files.
set(MAIN_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils.h
)

# Executable build.
add_executable(${PROJECT_NAME}
    # Main source files.
    ${MAIN_SOURCES}

    # stb header files.
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb/stb_image.h
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb/stb_image_resize2.h

    # Native extras.
    ${WIN64_RESOURCES}

    # Resource files.
    ${ASSET_DIR_FILES}
)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src/bt_lib
        ${DEPENDENCY_INCLUDE_DIRS})

target_link_libraries(${PROJECT_NAME}
    txp_renderer
    ${FMOD_LIBRARIES}
    fmt::fmt
    Jolt
    nlohmann_json::nlohmann_json
    stduuid
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.hawsoo.train_rail_simple_sim"
    MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
    MACOSX_BUNDLE_BUNDLE_VERSION "0.1.0"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1"
    # RESOURCE "${ASSET_DIR_FILES}"  @CHECK: commented these out since it caused resource files to get placed at the root of the bundle. So it should be good enough if changes?  -Thea 2026/02/08
)

# Set this project as Visual Studio startup project.
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
