#
# This function sets UBSAN and ASAN for the project (for GCC and Clang)
function(set_project_sanitizers project_name)
  option(ENABLE_UBSAN_AND_ASAN "Enable UBSAN and ASAN sanitizers" OFF)

  set(SANITIZERS
    "-fsanitize=address"
    "-fsanitize=undefined"
  )

  if(ENABLE_UBSAN_AND_ASAN)
    if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
        OR (CMAKE_CXX_COMPILER_ID STREQUAL "GNU"))
      target_compile_options(${project_name} INTERFACE ${SANITIZERS})
      target_link_options(${project_name} INTERFACE ${SANITIZERS})
    endif()
  endif()
endfunction()
