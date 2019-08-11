cmake_minimum_required(VERSION 3.1)

project(clam)

set(INSTALL_DIR /usr/local/bin/)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(EXTERN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/extern)
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CLAM_SRC_FILES "")

add_subdirectory(${SRC_DIR}) # populates CLAM_SRC_FILES

add_executable(clam "${CLAM_SRC_FILES}")

target_include_directories(clam PRIVATE ${SRC_DIR} ${EXTERN_DIR})

add_subdirectory(${EXTERN_DIR}) # Creates static clip library
set_target_properties(clip PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIB_DIR}/clip")

target_link_libraries(clam PRIVATE -ltomcrypt clip) # CXX_LINKER_FLAGS (ltomcrypt shared, clip static)

target_compile_options(clam PRIVATE -std=c++17) # CXX_COMPILE_FLAGS

set_target_properties(clam PROPERTIES
    LINKER_LANGUAGE CXX
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})

install(TARGETS clam DESTINATION ${INSTALL_DIR})
