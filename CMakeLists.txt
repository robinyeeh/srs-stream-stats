cmake_minimum_required(VERSION 3.13)
project(srs-stream-stats)

set(CMAKE_CXX_STANDARD 14)

SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

include_directories(./core ./kernal ./libs ./protocol ./app)

set(SRC_LIST main.cpp ./core/srs_core.cpp ./core/srs_core.hpp ./core/srs_core_performance.cpp ./core/srs_core_performance.hpp
        ./core/srs_core_autofree.cpp ./core/srs_core_autofree.hpp ./core/srs_core_mem_watch.cpp ./core/srs_core_mem_watch.hpp
        ./kernal/srs_kernel_codec.cpp ./kernal/srs_kernel_codec.hpp ./kernal/srs_kernel_buffer.cpp ./kernal/srs_kernel_buffer.hpp
        ./kernal/srs_kernel_error.cpp ./kernal/srs_kernel_error.hpp ./kernal/srs_kernel_file.cpp ./kernal/srs_kernel_file.hpp
        ./kernal/srs_kernel_flv.cpp ./kernal/srs_kernel_flv.hpp ./kernal/srs_kernel_flv.cpp ./kernal/srs_kernel_flv.hpp
        ./kernal/srs_kernel_stream.cpp ./kernal/srs_kernel_stream.hpp ./kernal/srs_kernel_utility.cpp ./kernal/srs_kernel_utility.hpp
        ./kernal/srs_kernel_log.cpp ./kernal/srs_kernel_log.hpp ./kernal/srs_kernel_consts.cpp ./kernal/srs_kernel_consts.hpp
        ./libs/srs_lib_bandwidth.cpp ./libs/srs_lib_bandwidth.hpp ./libs/srs_lib_simple_socket.cpp ./libs/srs_lib_simple_socket.hpp
        ./libs/srs_librtmp.cpp ./libs/srs_librtmp.hpp ./libs/srs_rtmp_utility.cpp ./libs/srs_rtmp_utility.hpp
        ./protocol/srs_raw_avc.cpp ./protocol/srs_raw_avc.hpp ./protocol/srs_rtmp_amf0.cpp ./protocol/srs_rtmp_amf0.hpp
        ./protocol/srs_rtmp_stack.cpp ./protocol/srs_rtmp_stack.hpp ./protocol/srs_rtmp_io.cpp ./protocol/srs_rtmp_io.hpp
        ./protocol/srs_protocol_buffer.cpp ./protocol/srs_protocol_buffer.hpp ./protocol/srs_rtmp_handshake.cpp ./protocol/srs_rtmp_handshake.hpp
        ./app/stream_stats.cpp ./app/stream_stats.hpp)

add_executable(srs-stream-stats ${SRC_LIST})