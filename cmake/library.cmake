
function (ez_add_library target_name lib_folder)
    file(GLOB_RECURSE header_files_ RELATIVE ${lib_folder} *.hpp)
    file(GLOB_RECURSE private_header_files_ RELATIVE ${lib_folder} *.hpp)
    file(GLOB_RECURSE source_files_ RELATIVE ${lib_folder} *.cpp)

    list(FILTER header_files_ INCLUDE REGEX "headers/(.*)")
    list(FILTER private_header_files_ INCLUDE REGEX "src/(.*)")
    list(FILTER source_files_ INCLUDE REGEX "src/(.*)")

    message("-- Adding library '${target_name}'")
    message("-- Header files '${header_files_}'")
    message("-- Private header files '${private_header_files_}'")
    message("-- Source files '${source_files_}'")


    add_library(${target_name} STATIC)

    target_sources(${target_name}
        PRIVATE
        ${header_files_}
        ${private_header_files_}
        ${source_files_}
    )

    target_include_directories(${target_name} PUBLIC ${lib_folder}/headers)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)

    add_sanitizers(${target_name})

endfunction()


function (ez_add_header_only_library target_name lib_folder)
    file(GLOB_RECURSE header_files_ RELATIVE ${lib_folder} *.hpp)

    list(FILTER header_files_ INCLUDE REGEX "headers/(.*)")

    message("-- Header files '${header_files_}'")

    add_library(${target_name} INTERFACE)

    target_sources(${target_name}
        PRIVATE
        ${header_files_}
    )

    target_include_directories(${target_name} INTERFACE ${lib_folder}/headers)
    set_target_properties(${target_name} PROPERTIES LINKER_LANGUAGE CXX)
endfunction()
