cmake_minimum_required(VERSION 3.31)
project(CitySmart VERSION 0.1.0 LANGUAGES C CXX)

# ===============================
# Create executable
# ===============================
add_executable(CitySmart main.cpp)

# ===============================
# Platform-specific Raylib setup
# ===============================
if(WIN32)
    # Windows MSVC
    set(RAYLIB_PATH "C:/raylib_path")
    set(RAYLIB_INCLUDE_DIR "${RAYLIB_PATH}/include")
    set(RAYLIB_LIBRARY "${RAYLIB_PATH}/lib/raylib.lib")
    set(RAYLIB_DLL "${RAYLIB_PATH}/lib/raylib.dll")

    target_include_directories(CitySmart PRIVATE ${RAYLIB_INCLUDE_DIR})
    target_link_libraries(CitySmart PRIVATE ${RAYLIB_LIBRARY} opengl32 gdi32 winmm)

    # Copy DLL to build folder automatically
    add_custom_command(TARGET CitySmart POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${RAYLIB_DLL}
        $<TARGET_FILE_DIR:CitySmart>)

elseif(APPLE)
    # macOS
    find_package(raylib REQUIRED)
    target_link_libraries(CitySmart PRIVATE raylib)

elseif(UNIX)
    # Linux
    find_package(raylib REQUIRED)
    target_link_libraries(CitySmart PRIVATE raylib)
endif()
