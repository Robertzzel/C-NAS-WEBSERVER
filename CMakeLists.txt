cmake_minimum_required(VERSION 3.28)
project(untitled C)

set(CMAKE_C_STANDARD 23)

find_package(OpenSSL REQUIRED)
find_package(LIBZIP REQUIRED)
find_package(ZLIB REQUIRED)

pkg_check_modules(LIBZIP REQUIRED libzip)
include_directories(${LIBZIP_INCLUDE_DIRS})

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
        file_utils/file_utils.c
        file_utils/file_utils.h
)

target_link_libraries(untitled OpenSSL::SSL OpenSSL::Crypto)
target_link_libraries(untitled ${LIBZIP_LIBRARIES})
target_link_libraries(untitled ZLIB::ZLIB)
target_compile_options(untitled PRIVATE -DOPENSSL_USE_STATIC_LIBS)
