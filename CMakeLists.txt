cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 23)

find_package(OpenSSL REQUIRED)

add_executable(untitled main.c
        IO/socket_t.c
        IO/socket_t.h
        http/http_request.c
        http/http_request.h
        utils/utils.c
        utils/utils.h
        http/http_response.c
        http/http_response.h
        html/html_files.c
        html/html_files.h
        routes/routes.h
        utils/file_utils.c
        utils/file_utils.h
        utils/zip.c
        utils/zip.h
        utils/file.h
        routes/login.c
        routes/root.c
        routes/not_found.c
        routes/static.c
        routes/download.c
        users/users.c
        users/users.h
        routes/home.c
        utils/file.c
        utils/string_t.h
        utils/string_t.c
        functional_utils/list.h
        functional_utils/list.c
)
add_compile_options(-Wall)
target_link_libraries(untitled OpenSSL::SSL OpenSSL::Crypto)
target_compile_options(untitled PRIVATE -DOPENSSL_USE_STATIC_LIBS)
