cmake_minimum_required(VERSION 3.15)
project(RubikX3D CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB_RECURSE SRC_FILES 
    "src/*.cpp"
)

# Header search paths
include_directories(src)
include_directories(src/algorithms)
include_directories(src/cube)
include_directories(src/solver)
include_directories(src/ui)
include_directories(src/utils)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLUT (macOS uses framework, Linux/Windows use package)
if(APPLE)
    # Check for GLUT framework
    find_library(GLUT_LIBRARY GLUT)
    find_library(OPENGL_LIBRARY OpenGL)
    set(GLUT_LIBRARIES ${GLUT_LIBRARY})
    set(OPENGL_LIBRARIES ${OPENGL_LIBRARY})
else()
    find_package(GLUT REQUIRED)
    include_directories(${GLUT_INCLUDE_DIR})
endif()

add_executable(RubikX3D ${SRC_FILES})

if(APPLE)
    target_link_libraries(RubikX3D 
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARIES}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
    )
    # Disable deprecation warnings for GLUT on macOS
    target_compile_options(RubikX3D PRIVATE -Wno-deprecated-declarations)
else()
    target_link_libraries(RubikX3D 
        ${OPENGL_LIBRARIES} 
        ${GLUT_LIBRARIES}
    )
endif()
