
function (ez_add_library target_name lib_folder)
    file(GLOB_RECURSE header_files_ RELATIVE ${lib_folder} *.hpp)
    file(GLOB_RECURSE private_header_files_ RELATIVE ${lib_folder} *.hpp)
    file(GLOB_RECURSE source_files_ RELATIVE ${lib_folder} *.cpp)

    list(FILTER header_files_ INCLUDE REGEX "headers/(.*)")
    list(FILTER private_header_files_ INCLUDE REGEX "src/(.*)")
    list(FILTER source_files_ INCLUDE REGEX "src/(.*)")


    message("++++ Header files '${header_files_}'")
    message("++++ Private header files '${private_header_files_}'")
    message("++++ Source files '${source_files_}'")


    add_library(${target_name} STATIC)

    target_sources(ez_core
        PRIVATE
        ${header_files_}
        ${private_header_files_}
        ${source_files_}
    )

    target_include_directories(${target_name} PUBLIC ${lib_folder}/headers)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)
    target_compile_features(${target_name} PUBLIC cxx_std_23)
endfunction()
