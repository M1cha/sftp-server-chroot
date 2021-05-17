cmake_minimum_required(VERSION 2.8)
project(sftp_server_chroot C)

add_compile_options(
    -Wall
    -Wextra
    -Wshadow
    -Werror
)

add_library(sftp_server_chroot_preload SHARED
    preload.c
)
