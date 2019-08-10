cmake_minimum_required(VERSION 3.1)

project(clam)

set(INSTALL_DIR /usr/local/bin/)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CXX_LINKER_FLAGS -ltomcrypt)
set(CXX_COMPILE_FLAGS -std=c++17)

set(CLAM_SRC_FILES "")

add_subdirectory(${SRC_DIR}) # populates CLAM_SRC_FILES

add_executable(clam "${CLAM_SRC_FILES}")

target_include_directories(clam PRIVATE ${INCLUDE_DIR})

target_link_libraries(clam PRIVATE ${CXX_LINKER_FLAGS})

target_compile_options(clam PRIVATE ${CXX_COMPILE_FLAGS})

set_target_properties(clam PROPERTIES
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

install(TARGETS clam DESTINATION ${INSTALL_DIR})
