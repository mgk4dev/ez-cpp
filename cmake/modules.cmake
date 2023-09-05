set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "aa1f7df0-828a-4fcd-9afc-2dc80491aca7")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)

# Default to C++ extensions being off. Clang's modules support have trouble
# with extensions right now.
set(CMAKE_CXX_EXTENSIONS OFF)

# clang-scan-deps is not looking to the right include path
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/opt/local/libexec/llvm-16/lib/clang/16/include/")


include_directories("/opt/local/libexec/llvm-16/lib/clang/16/include/")

function (add_module_library target_name)
    add_library(${target_name})
    target_sources(
            ${target_name}
            PUBLIC
                FILE_SET cxx_modules TYPE CXX_MODULES FILES
                ${ARGN}
        )
endfunction()



