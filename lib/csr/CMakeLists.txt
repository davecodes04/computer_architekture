aux_source_directory(. DIR_csr_SRCS)

include_directories(.)

add_library(csr ${DIR_csr_SRCS})
# target_include_directories(csr PUBLIC hardware_regs hardware_regs_headers)
target_link_libraries(csr PUBLIC pico_stdlib hardware_regs hardware_regs_headers hardware_base)
