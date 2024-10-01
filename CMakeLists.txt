cmake_minimum_required(VERSION 3.28)
project(SrvMonitor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE DEBUG)

# set include directories
set(SrcDIR ${CMAKE_SOURCE_DIR}/src)
set(IncludeDIR ${CMAKE_SOURCE_DIR}/include)
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)

include_directories(
        ${SrcDIR}
        ${IncludeDIR}
)

file(GLOB_RECURSE SOURCE_FILES ${SrcDIR}/*.cpp ${SrcDIR}/*.c ${SrcDIR}/**/*.cpp ${SrcDIR}/**/*.c)
file(GLOB_RECURSE HEADER_FILES ${SrcDIR}/*.h ${SrcDIR}/*.hpp ${SrcDIR}/**/*.h ${SrcDIR}/**/*.hpp)

add_executable(srvmonitor ${CMAKE_SOURCE_DIR}/src/main.cpp ${SOURCE_FILES})
target_link_directories(srvmonitor PUBLIC ${CMAKE_SOURCE_DIR}/src/)

install(TARGETS srvmonitor DESTINATION ${CMAKE_INSTALL_BINDIR})