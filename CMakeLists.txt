cmake_minimum_required(VERSION 3.20)
project(OfflineRenderer LANGUAGES CXX)

find_package(Threads REQUIRED)

add_executable(offline_renderer
    src/Main.cpp
    src/Renderer.cpp
    src/Scene.cpp
    src/BVH.cpp
)
target_compile_features(offline_renderer PRIVATE cxx_std_17)
target_link_libraries(offline_renderer PRIVATE Threads::Threads)
if(MSVC)
    target_compile_options(offline_renderer PRIVATE /utf-8)
endif()
set_target_properties(offline_renderer PROPERTIES
    CXX_EXTENSIONS OFF
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)

# Keep scene assets in the build directory for single- and multi-config builds.
foreach(model floor shortbox tallbox left right light)
    configure_file(
        "src/models/cornellbox/${model}.obj"
        "models/cornellbox/${model}.obj"
        COPYONLY
    )
endforeach()
