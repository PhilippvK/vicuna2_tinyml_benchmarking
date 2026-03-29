
####
#Add bsp library for Vicuna
####
add_library(bsp_Vicuna STATIC)

target_include_directories(bsp_Vicuna PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(bsp_Vicuna PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/crt0.S
)


add_library(UART_Vicuna STATIC)

target_include_directories(UART_Vicuna PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

target_sources(UART_Vicuna PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/uart.h
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/uart.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/vicuna_crt.h
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/vicuna_crt.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/terminate_benchmark.h
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/terminate_benchmark.c
  ${CMAKE_CURRENT_SOURCE_DIR}/lib/trap_handler.c
)


###
#Add libwrap support for syscall-based semihosting
###

# include(CMakePrintHelpers)
set(LIB_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/_exit.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/write.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/_write.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/fstat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/_fstat.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/isatty.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/_isatty.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/sbrk.c
    ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/_sbrk.c
    # TODO:
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/close.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/execve.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/fork.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/getpid.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/kill.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/link.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/lseek.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/open.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/openat.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/printf.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/puts.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/read.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/stat.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/times.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/unlink.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/wait.c
    # ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/stdlib/malloc.c
)
set(WRAP_ARGS "")
foreach(FILE ${LIB_SOURCES})
    get_filename_component(DIR ${FILE} DIRECTORY)
    get_filename_component(BASE_NAME ${FILE} NAME_WE)
    list(APPEND WRAP_ARGS "LINKER:--wrap=${BASE_NAME}")
endforeach()

add_library(libwrap_Vicuna STATIC)

target_include_directories(libwrap_Vicuna PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/lib
  ${CMAKE_CURRENT_SOURCE_DIR}/libwrap
)

target_sources(libwrap_Vicuna PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/libwrap/sys/_exit.c
  ${LIB_SOURCES}
)

# add_library(libwrap_Vicuna STATIC ${LIB_SOURCES} ../env/${BOARD}/bsp_write.c ../env/${BOARD}/bsp_read.c)
target_include_directories(libwrap_Vicuna PUBLIC ../include)
target_link_options(libwrap_Vicuna INTERFACE ${WRAP_ARGS})

target_link_libraries(bsp_Vicuna INTERFACE libwrap_Vicuna)

####
#Add bsp library for Spike Baremetal
####
add_library(bsp_Spike STATIC)

target_include_directories(bsp_Spike PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Spike_Support
)
target_sources(bsp_Spike PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Spike_Support/crt0.S
  ${CMAKE_CURRENT_SOURCE_DIR}/Spike_Support/terminate_benchmark.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Spike_Support/uart.h
  ${CMAKE_CURRENT_SOURCE_DIR}/Spike_Support/terminate_benchmark.c
)


