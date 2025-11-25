cmake_minimum_required(VERSION 3.31)
project(CitySmart VERSION 0.1.0 LANGUAGES C CXX)

# ===============================
# Use C++17
# ===============================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===============================
# Create executable
# ===============================
add_executable(CitySmart main.cpp)

# ===============================
# Platform-specific Raylib setup
# ===============================
if (WIN32)

    if(MSVC)
        message(STATUS "Using MSVC")
        set(RAYLIB_PATH "C:/raylib_path")
        set(RAYLIB_LIB "${RAYLIB_PATH}/lib/raylib.lib")
        set(RAYLIB_DLL "${RAYLIB_PATH}/lib/raylib.dll")

        target_include_directories(CitySmart PRIVATE "${RAYLIB_PATH}/include")
        target_link_libraries(CitySmart PRIVATE ${RAYLIB_LIB} opengl32 gdi32 winmm)

        add_custom_command(TARGET CitySmart POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${RAYLIB_DLL}
            $<TARGET_FILE_DIR:CitySmart>)
    
    elseif(MINGW)
        message(STATUS "Using MinGW")
        set(RAYLIB_PATH "C:/raylib_mingw")
        set(RAYLIB_LIB "${RAYLIB_PATH}/lib/libraylib.a")

        target_include_directories(CitySmart PRIVATE "${RAYLIB_PATH}/include")
        target_link_libraries(CitySmart PRIVATE ${RAYLIB_LIB} opengl32 gdi32 winmm)
    endif()

elseif(APPLE)
    # macOS (Homebrew)
    find_package(raylib REQUIRED)
    target_link_libraries(CitySmart PRIVATE raylib)

elseif(UNIX)
    # Linux
    find_package(raylib REQUIRED)
    target_link_libraries(CitySmart PRIVATE raylib)

endif()