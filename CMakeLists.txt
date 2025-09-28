cmake_minimum_required(VERSION 3.20)
project(opengl_glfw_macos LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# If you use Apple Silicon, this is usually detected automatically.
# You can force a build arch if needed:
# set(CMAKE_OSX_ARCHITECTURES "arm64")  # or "x86_64"

# Add GLAD (vendored)
add_library(glad vendor/glad/src/glad.c)
target_include_directories(glad PUBLIC vendor/glad/include)

# Find GLFW installed via Homebrew
# brew installs a CMake config, so this should work:
find_package(glfw3 REQUIRED)

# OpenGL on macOS
find_package(OpenGL REQUIRED)


file(GLOB SRC_FILES CONFIGURE_DEPENDS src/*.cpp)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_executable(app ${SRC_FILES})

target_link_libraries(app
  PRIVATE
    glad
    glfw
    OpenGL::GL
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
)

# Nice warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
  target_compile_options(app PRIVATE -Wall -Wextra -Wpedantic)
endif()
