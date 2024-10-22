cmake_minimum_required(VERSION 3.4)

project(inexor-cube2-map-importer)

file(GLOB_RECURSE source_files
  "src/*.hpp"
  "src/*.cpp"
)

# Dependency setup via conan.
# Download conan executer in case it does not exists.
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.14/conan.cmake"
                 "${CMAKE_CURRENT_BINARY_DIR}/conan.cmake")
endif()

find_library(ZLIB zlib)

# Execute conan build instructions.
include(${CMAKE_CURRENT_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE conanfile.py
                BASIC_SETUP
                BUILD outdated
                PROFILE default
                PROFILE_AUTO build_type
                KEEP_RPATHS
)

add_executable(inexor-cube2-map-importer src/main.cpp ${source_files})

target_link_libraries(inexor-cube2-map-importer PUBLIC ${CONAN_LIBS})
