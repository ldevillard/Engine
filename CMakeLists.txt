cmake_minimum_required(VERSION 3.10)

# recursive organisation of files
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${PROJECT_SOURCE_DIR}" "${_source}")
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

# project name
project(DevilEngine)

# include directories
include_directories(${PROJECT_SOURCE_DIR}/Engine/include)
include_directories(${PROJECT_SOURCE_DIR}/ThirdPartyLibs/ImGui)
include_directories(${PROJECT_SOURCE_DIR}/ThirdPartyLibs/ImGuizmo)
include_directories(${PROJECT_SOURCE_DIR}/ThirdPartyLibs/Include)

# source files
file(GLOB_RECURSE SOURCES "Engine/src/*.cpp" "Engine/src/*.c" "Engine/include/*.h" "Engine/shaders/*.glsl" "Engine/shaders/*.vs" "Engine/shaders/*.fs" "ThirdPartyLibs/*.cpp" "ThirdPartyLibs/*.h")

# Définir le répertoire de sortie pour les DLL
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# setup C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# setup parallel compiling
if(MSVC)
    add_compile_options(/MP)
endif()

set(CMAKE_SUPPRESS_REGENERATION true)

# setup .exe
add_executable(DevilEngine ${SOURCES} Engine/resources/DevilEngine.rc)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_target_properties(DevilEngine PROPERTIES
    PROJECT_LABEL "DevilEngine"
    FOLDER "Engine"
)

# organise visual studio files
assign_source_group(${SOURCES})

# link external libraries
target_link_libraries(DevilEngine ${PROJECT_SOURCE_DIR}/ThirdPartyLibs/Libs/glfw3.lib)
target_link_libraries(DevilEngine ${PROJECT_SOURCE_DIR}/ThirdPartyLibs/Libs/assimp-vc143-mt.lib)

# copy DLL to output directory
add_custom_command(TARGET DevilEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${PROJECT_SOURCE_DIR}/ThirdPartyLibs/bin/assimp-vc143-mt.dll"
    $<TARGET_FILE_DIR:DevilEngine>
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:DevilEngine>/shaders
    COMMAND ${CMAKE_COMMAND} -E remove_directory
    $<TARGET_FILE_DIR:DevilEngine>/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/Engine/shaders"
    $<TARGET_FILE_DIR:DevilEngine>/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/Engine/resources"
    $<TARGET_FILE_DIR:DevilEngine>/resources
    COMMAND ${CMAKE_COMMAND} -E copy
    "${PROJECT_SOURCE_DIR}/Engine/imgui.ini"
    $<TARGET_FILE_DIR:DevilEngine>
)

# set up solution directories
set_target_properties(DevilEngine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/bin/Debug
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin/Release
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DevilEngine)

# Exclure DevilEngine du groupe ALL_BUILD
set_target_properties(DevilEngine PROPERTIES EXCLUDE_FROM_ALL TRUE)

# Spécifier le chemin du fichier projet DevilEngine
set_property(TARGET DevilEngine PROPERTY FOLDER "Engine")

set(NEW_INTERMEDIATE_DIR "${CMAKE_BINARY_DIR}/Engine/bin")

# Appliquer cette configuration au projet DevilEngine
set_target_properties(DevilEngine PROPERTIES
    INTERMEDIATE_DIRECTORY "${NEW_INTERMEDIATE_DIR}"
)