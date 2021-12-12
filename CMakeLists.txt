cmake_minimum_required(VERSION 3.16)
project(msg_logging)

set(CMAKE_CXX_STANDARD 17)

set(generated_dir ${CMAKE_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${generated_dir})
include_directories(${generated_dir} ${CMAKE_SOURCE_DIR})
message("** generated_dir............: ${generated_dir}")
message("** source dir............: ${CMAKE_SOURCE_DIR}")
add_custom_command(
        OUTPUT ${generated_dir}/log_msg_generated.h
        COMMAND flatc --cpp --scoped-enums --grpc ${CMAKE_SOURCE_DIR}/log_msg.fbs
        WORKING_DIRECTORY ${generated_dir}
        DEPENDS ${CMAKE_SOURCE_DIR}/log_msg.fbs
)
set_source_files_properties(${generated_dir}/log_msg_buf.h
        PROPERTIES GENERATED TRUE)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION true)
    set(CMAKE_CXX_FLAGS "-g3 -Wall -fuse-linker-plugin -O3 -DNDEBUG")
endif ()

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# compile the flatbuffer file
add_custom_target(flat_compiled_headers ALL
        DEPENDS ${generated_dir}/log_msg_generated.h
        )

add_library(flat_msg OBJECT lib/LogEntrySerializer.cc lib/LogEntrySerializer.h
        ${generated_dir}/log_msg_generated.h
        ${generated_dir}/log_msg.grpc.fb.cc
        )
add_dependencies(flat_msg flat_compiled_headers)

add_library(zmq_components OBJECT
        lib/publisher.h
        lib/subscriber.h
        lib/pub_sub_proxy.h
        lib/binding.h)
target_include_directories(zmq_components PRIVATE lib)
set_target_properties(zmq_components PROPERTIES LINKER_LANGUAGE CXX)

add_executable(msg_logging main.cpp)
add_dependencies(msg_logging flat_msg)
target_include_directories(msg_logging BEFORE PRIVATE lib)
target_link_libraries(msg_logging ${CONAN_LIBS} flat_msg)

add_executable(zmq_server
        zmq_consumer/zmq_log_server.cc
        zmq_consumer/zmq_consumer.cc)
target_link_libraries(zmq_server ${CONAN_LIBS} flat_msg)

add_executable(try_pub_sub_zmq_proxy
        pub_sub_zmq_proxy/pub_sub_zmq_proxy_main.cc)
target_include_directories(try_pub_sub_zmq_proxy PRIVATE lib)
target_link_libraries(try_pub_sub_zmq_proxy ${CONAN_LIBS} flat_msg zmq_components)

add_executable(try_pull_sub_zmq_proxy
        pull_sub_zmq_proxy/pull_sub_zmq_proxy_main.cc)
target_include_directories(try_pull_sub_zmq_proxy PRIVATE lib)
target_link_libraries(try_pull_sub_zmq_proxy ${CONAN_LIBS} flat_msg zmq_components)

add_executable(benchmarks bench/benchmarks.cc)
target_include_directories(benchmarks BEFORE PRIVATE lib)
target_link_libraries(benchmarks ${CONAN_LIBS} flat_msg)

add_executable(grpc_benchmarks bench/grpc_bench.cc)
target_include_directories(grpc_benchmarks BEFORE PRIVATE lib)
target_link_libraries(grpc_benchmarks ${CONAN_LIBS} flat_msg)

add_executable(try_grpc_server
        grpc_server/grpc_server_main.cc)
target_include_directories(try_grpc_server BEFORE PRIVATE lib)
target_link_libraries(try_grpc_server ${CONAN_LIBS} flat_msg)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/)
