cmake_minimum_required(VERSION 3.18)

set(TARGET DekitaRPG)
project(${TARGET})

set(ModOutputDir ${CMAKE_SOURCE_DIR}/Output/${TARGET})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ModOutputDir})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ModOutputDir})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ModOutputDir})

set(BASE_SOURCE ${CMAKE_SOURCE_DIR}/DekitaMod/src)

set(${TARGET}_SOURCES 
    ${BASE_SOURCE}/dllmain.cpp
    ${BASE_SOURCE}/Utils.hpp
    ${BASE_SOURCE}/Utils.cpp
    ${BASE_SOURCE}/Kore.hpp
    ${BASE_SOURCE}/Kore.cpp
    ${BASE_SOURCE}/BPFL.hpp
    ${BASE_SOURCE}/BPFL.cpp
    ${BASE_SOURCE}/IFace.hpp
    ${BASE_SOURCE}/IFace.cpp
)

add_library(${TARGET} SHARED ${${TARGET}_SOURCES})
target_include_directories(${TARGET} PRIVATE "src")
target_link_libraries(${TARGET} PUBLIC cppsdk xinput1_3)