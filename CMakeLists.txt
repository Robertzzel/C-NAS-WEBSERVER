cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 23)

find_package(OpenSSL REQUIRED)

add_executable(untitled main.c
        socket_t.h
        socket_t.c
        http/http_request.c
        http/http_request.h
        utils/utils.c
        utils/utils.h
        http/http_response.c
        http/http_response.h
        html/html_files.c
        html/html_files.h
        ctemplate/ctemplate.c
        ctemplate/ctemplate.h
        routes/routes.h
        utils/file_utils.c
        utils/file_utils.h
        utils/zip.c
        utils/zip.h
        routes/login.c
        routes/root.c
        routes/not_found.c
        routes/static.c
        routes/download.c
        users/users.c
        users/users.h
)
add_compile_options(-Wall)
target_link_libraries(untitled OpenSSL::SSL OpenSSL::Crypto)
target_compile_options(untitled PRIVATE -DOPENSSL_USE_STATIC_LIBS)
