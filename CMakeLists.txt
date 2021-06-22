cmake_minimum_required(VERSION 3.15)
project(TP C)

set(CMAKE_C_STANDARD 11)

include_directories(.)
include_directories(read_lib)

add_executable(TP
        read_lib/read_lib.c
        read_lib/read_lib.h
        data.c
        data.h
        proto.c
        proto.h
        reqRep.h
        session.c
        session.h
        StreamCltSrv.c)
