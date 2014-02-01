# This cmake file will pick up any file called *-main.cpp and build a target for it.
set(CMAKE_C_COMPILER_INIT g++)

PROJECT(examples)

cmake_minimum_required(VERSION 2.8)

file(GLOB files "*-main.cpp")

include_directories("${PROJECT_SOURCE_DIR}")

foreach(filePath ${files})
  get_filename_component(file ${filePath} NAME)
  STRING(REGEX REPLACE 
    "([a-zA-Z]+)-main.cpp" "\\1" 
    executable
    ${file}
  )
  add_executable(${executable} ${filePath})
  MESSAGE("Creating build for ${executable}")
endforeach()


SET(GCC_COVERAGE_COMPILE_FLAGS "-g")