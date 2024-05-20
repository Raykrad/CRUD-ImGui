cmake_minimum_required(VERSION 3.20)
project(database VERSION 0.0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(OpenGL_GL_PREFERENCE LEGACY)

find_package(OpenGL REQUIRED)
add_subdirectory(libraries)

add_executable(database main.cpp includes/includes.h includes/license.cpp includes/license.h includes/po.cpp includes/po.h includes/user.cpp includes/user.h includes/hub.cpp includes/hub.h)

target_include_directories(database
    PUBLIC
    libraries
    includes
    ${OPENGL_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(database
    PUBLIC
    ${OPENGL_gl_LIBRARY}
    glfw
    imgui
)

include(GNUInstallDirs)

install(TARGETS database
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
