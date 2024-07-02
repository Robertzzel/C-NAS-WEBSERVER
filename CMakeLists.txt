cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 23)

find_package(OpenSSL REQUIRED)

add_executable(untitled main.c
        s_socket.h
        s_socket.c
        http_request.c
        http_request.h
        m_string.c
        m_string.h
        utils.c
        utils.h
        error.h
)

target_link_libraries(untitled OpenSSL::SSL OpenSSL::Crypto)
target_compile_options(untitled PRIVATE -DOPENSSL_USE_STATIC_LIBS)
