cmake_minimum_required(VERSION 3.10.0)
project(tegen VERSION 0.1.0 LANGUAGES C CXX)

# -------------------------
# C++ settings
# -------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Executable
# -------------------------
add_executable(tegen src/main.cpp)
target_include_directories(tegen PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_compile_definitions(PACKAGE_VERSION="${PROJECT_VERSION}")

# -------------------------
# Install target (for Linux)
# -------------------------
# Installs the executable to /usr/local/bin so it's in PATH
install(TARGETS tegen DESTINATION bin)

# -------------------------
# CPack configuration
# -------------------------
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_CONTACT "Mickyas Tesfaye alazartesfaye42@gmail.com")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Mickyas Tesfaye alazartesfaye42@gmail.com")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "") # Add Linux dependencies here if needed

include(CPack)
