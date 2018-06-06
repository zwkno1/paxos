cmake_minimum_required(VERSION 2.8)

project(paxos)

set (CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS   ON)
find_package(Boost 1.66 COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Threads)

include_directories(./)

add_library(paxos acceptor.cpp  commdata.cpp  leveldb.cpp  proposer.cpp acceptorproxy.cpp proposerproxy.cpp)

target_link_libraries(paxos ${Boost_LIBRARIES}
	${CMAKE_THREAD_LIBS_INIT} 
	)
