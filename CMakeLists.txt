cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 23)

find_package(OpenSSL REQUIRED)

add_executable(untitled main.c
        s_socket.h
        s_socket.c
        http/http_request.c
        http/http_request.h
        utils.c
        utils.h
        error.h
        http/http_response.c
        http/http_response.h
        html/html_files.c
        html/html_files.h
        ctemplate/ctemplate.c
        ctemplate/ctemplate.h
        routes/routes.c
        routes/routes.h
)

target_link_libraries(untitled OpenSSL::SSL OpenSSL::Crypto)
target_compile_options(untitled PRIVATE -DOPENSSL_USE_STATIC_LIBS)
