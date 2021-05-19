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

add_custom_target(
    dist
    COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/dist
    COMMAND cp $<TARGET_FILE:sftp_server_chroot_preload> ${CMAKE_CURRENT_BINARY_DIR}/dist/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/sftp-server-chroot ${CMAKE_CURRENT_BINARY_DIR}/dist/
    DEPENDS sftp_server_chroot_preload
    )
