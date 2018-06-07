cmake_minimum_required(VERSION 2.8)

project(paxos)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

set (CMAKE_CXX_STANDARD 17)

find_package(LevelDB)
include_directories(${LEVELDB_INCLUDE_DIRS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.66 COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Threads)

include_directories(./)

add_library(paxos acceptor.cpp  commdata.cpp  leveldb.cpp  proposer.cpp acceptorproxy.cpp proposerproxy.cpp logger.cpp)

target_link_libraries(paxos ${Boost_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT} 
	${LEVELDB_LIBRARIES}
	${JSONCPP_LIBRARIES}
	)

add_executable(server main.cpp)
add_executable(initdb initdb.cpp)

target_link_libraries(server paxos)
target_link_libraries(initdb paxos)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/initdb.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

