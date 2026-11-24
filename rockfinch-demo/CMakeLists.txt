cmake_minimum_required(VERSION 3.28)
project(rockfinch_demo CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(SPARROW_ROCKFINCH_SOURCE_DIR "" CACHE PATH "sparrow-rockfinch source tree")

find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
execute_process(COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir OUTPUT_STRIP_TRAILING_WHITESPACE OUTPUT_VARIABLE nanobind_ROOT)
find_package(nanobind CONFIG REQUIRED)
find_package(xtensor CONFIG REQUIRED)
set(ROCKFINCH_DEMO_INSTALL_DIR "${Python_SITEARCH}" CACHE PATH "Install directory of the Python module")

if(EMSCRIPTEN)
    # The Emscripten toolchain turns MODULE libraries into static archives by default;
    # the -sSIDE_MODULE=1 link flag comes from emscripten-forge's LDFLAGS
    set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
endif()

# sparrow-rockfinch's C++ library, linked statically; its own Python module is not needed
set(SPARROW_ROCKFINCH_BUILD_SHARED OFF CACHE BOOL "" FORCE)
set(SPARROW_ROCKFINCH_BUILD_PYTHON_MODULE OFF CACHE BOOL "" FORCE)
add_subdirectory(${SPARROW_ROCKFINCH_SOURCE_DIR} sparrow-rockfinch EXCLUDE_FROM_ALL)

nanobind_add_module(rockfinch_demo NB_STATIC rockfinch_demo.cpp)
target_link_libraries(rockfinch_demo PRIVATE sparrow-rockfinch-cpp xtensor)

install(TARGETS rockfinch_demo LIBRARY DESTINATION ${ROCKFINCH_DEMO_INSTALL_DIR})
